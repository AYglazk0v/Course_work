#include "../includes/steps2hell.h"
#include "../includes/funcRecvRespList.h"
#include "../includes/get_next_line_bonus.h"
#include "../includes/listOfFiles.h"
#include "../includes/timingModes.h"
#include "../includes/funcRecvRespList.h"

//Прототип функции для устранения конфликта во время рекурсии.
static void	fn_solver(char *c, t_info *inf);

/*
#	Функция для инициализации контекста SSL
*/
static void	fn_initServerCTX(t_info *inf)
{
	OpenSSL_add_all_algorithms(); // Инициализация крипто функций
	SSL_load_error_strings(); // Загрузить сообщение об ошибке SSL

	inf->ctx = SSL_CTX_new(TLS_server_method()); //Создание нового контекста SSL
	if (!inf->ctx)
	{
		ERR_print_errors_fp(stderr); //Вывод ошибки создания контекста в поток ошибки
		exit(EXIT_FAILURE);
	}
}

/*
#	Функция для загрузки и настройки проверки верификации по сертификатам
*/
static void	fn_loadCertificates(t_info *inf, char *CertFile, char *KeyFile, char *RootFile)
{
	/*
	//Установка флагов проверки для ctx.
	//SSL_VERIFY_PEER -- сервер отправляет клиенту запрос сертификата клиента.
	//Возвращенный сертификат (если есть) проверяется. Если процесс проверки происходит сбой,
	// TLS/SSL немедленно завершается с предупреждающим сообщением, 
	//содержащим причину сбоя проверки.
	*/
	SSL_CTX_set_verify(inf->ctx, SSL_VERIFY_PEER, NULL);
	//установка расположение по умолчанию для доверенных сертификатов CA (корневой сертификат)
	if (SSL_CTX_load_verify_locations(inf->ctx, RootFile, NULL) == 0) 
	{
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
	//установка списка CA, отправляемых клиенту.
	//SSL_load_client_CA_file -- загрузка имен субъектов сертификата из файла
	SSL_CTX_set_client_CA_list(inf->ctx, SSL_load_client_CA_file(RootFile));
	//Загружает сертификат из файла в ctx 
	if (SSL_CTX_use_certificate_file(inf->ctx, CertFile, SSL_FILETYPE_PEM) != 1 )
	{
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
	//Установка закрытого ключа из файла в ctx
	if (SSL_CTX_use_PrivateKey_file(inf->ctx, KeyFile, SSL_FILETYPE_PEM) != 1 )
	{
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}
	//Проверка согласованности закрытого ключа с соответсвующим сертификатом загружаемым в ctx
	if (SSL_CTX_check_private_key(inf->ctx) != 1)
	{
		perror(RED"\t[-]Private key does not match the public certificate");
		exit(EXIT_FAILURE);
	}
	//Установка режима чрез битовую маску, SSL_MODE_AUTO_RETRY -- write/read будут работать только после рукопожатия
	SSL_CTX_set_mode(inf->ctx, SSL_MODE_AUTO_RETRY);
	//Если клиент не вернул сертификат, TLS/SSL немедленно завершается с ошибкой рукопожатия.
	SSL_CTX_set_verify(inf->ctx,
						SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
						NULL);
	//Установка максимальной глубины проверки цепочки сертификатор, которая должна быть разрешена для ctx
	SSL_CTX_set_verify_depth(inf->ctx, 1);
}

/*
#	Функция печатающая сетевой интерфейс и IP адрес устройства
*/
static void	fn_printWlanIP(void)
{
	struct ifaddrs	*ifaddr, *ifa;
	int				s, key;
	char			host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) //получить адрес интерфесов
	{
		perror(RED"\tgetifaddrs");
		exit(EXIT_FAILURE);
	}
	key = 0;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;
		//преобразование адреса в имя в независимом от протокола
		s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		if((strcmp(ifa->ifa_name,"wlan0") == 0) && (ifa->ifa_addr->sa_family == AF_INET) && s == 0)
		{
			key = 1;
			printf(YELLOW"\t[!]Interface %s/\n",ifa->ifa_name);
			printf(YELLOW"\t[!]HOST IP %s\n"RESET, host);
			printf(YELLOW "\t[!]PORT 3389\n"RESET);
		}
	}
	freeifaddrs(ifaddr);
	if (!key)
	{
		printf(RED"\t[!]Interfase not found\n"RESET);
		exit(EXIT_FAILURE);
	}
}

/*
#	Функция инициализирующая сокет и прослушиваение порта
*/
static void	fn_openListener(t_info *inf)
{
	fn_printWlanIP();

	//Создаем сокет
	if ((inf->fd_serv = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror(RED"\t[-]Socket");
		exit(EXIT_FAILURE);
	}
	printf(GREEN"\t[+]Socket created\n"RESET);

	setsockopt(inf->fd_serv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
	bzero(&inf->server_addr, sizeof(inf->server_addr));
	inf->server_addr.sin_family = AF_INET;
	inf->server_addr.sin_port = htons(PORT);
	inf->server_addr.sin_addr.s_addr = INADDR_ANY;

	//Связываем сокет с конкретным адресом
	if (bind(inf->fd_serv, (struct sockaddr*)&inf->server_addr,
							 sizeof(struct sockaddr)) != 0)
	{
		perror(RED"\t[-]Can't bind port");
		exit(EXIT_FAILURE);
	}
	printf(GREEN"\t[+]Binded\n"RESET);

	//Подготавливает привязываемый сокет к принятию входящих соединений
	if (listen(inf->fd_serv, 1) != 0)
	{
		perror(RED"\t[-]Can't configure listening port");
		exit(EXIT_FAILURE);
	}
	printf(GREEN"\t[+]Begin listen\n"RESET);
}

/*
#	Функция для перехода (создания, если не было) в директорию синхронизации
*/
static void	fn_GetDirPhone(t_info *inf)
{
	char	**dirs;
	int		i;
	int		status;

	bzero(inf->buff, MAXBUF);
	SSL_read(inf->ssl, inf->buff, MAXBUF);
	i = -1;
	dirs = fn_split(inf->buff, '/');
	while (dirs[++i])
	{
		status = access(dirs[i], F_OK);
		if (status < 0 && fn_existDir(dirs[i]) < 0)
		{
			if(mkdir(dirs[i], 0755) == -1)
			{
				perror(RED"\t[-]Directory creation error");
				exit(EXIT_FAILURE);
			}
		}
		else if (status == 0 && fn_existDir(dirs[i]) < 0)
		{
			remove(dirs[i]);
			if(mkdir(dirs[i], 0755) == -1)
			{
				perror(RED"\t[-]Directory creation error");
				exit(EXIT_FAILURE);
			}
		}
		chdir(dirs[i]);
	}
	fn_cleanDict(dirs);
}

/*
#	Функция для получения файла .c_files с клиента
*/
static void	fn_receive_c_file(t_info *inf)
{
	int		len;

	inf->fd = open(".c_files", O_RDWR | O_CREAT | O_TRUNC, 00666);
	if (inf->fd < 0)
	{
		perror(RED" .c_files");
		exit(EXIT_FAILURE);
	}
	SSL_write(inf->ssl, "give_c", 7);
	bzero(inf->buff, MAXBUF);
	printf(YELLOW"\t[!]recording .c_file started\n"RESET);
	while (1)
	{
		len = SSL_read(inf->ssl, inf->buff, MAXBUF);
		if (len == 1 && inf->buff[0] == 0)
			break;
		if (write(inf->fd, inf->buff, len) < 0)
		{
			perror(RED" write");
			exit(1);
		}
		if (len < MAXBUF)
		{
			printf(GREEN"\t[+]receive .c_file complete!\n"RESET);
			close(inf->fd);
			break;
		}
	}
}

/*
#	Функция для получения файла с клиента на сервер
*/
static void	fn_receivingFiles(t_listRec *curr, t_info *inf)
{
	int		len;

	while(curr)
	{
		SSL_write(inf->ssl, "send", 5);
		SSL_write(inf->ssl, curr->data, strlen(curr->data));
		inf->fd = open(curr->data, O_RDWR | O_CREAT | O_TRUNC, 00666);
		if(inf->fd < 0)
		{
			perror(curr->data);
			exit(EXIT_FAILURE);
		}
		printf(YELLOW"\t[!]recording %s started\n"RESET, curr->data);
		while (1)
		{
			bzero(inf->buff, MAXBUF);
			len = SSL_read(inf->ssl, inf->buff, MAXBUF);
			if (len == 1 && inf->buff[0] == 0)
				break;
			if (write(inf->fd, inf->buff, len) < 0)
			{
				perror("write");
				exit(EXIT_FAILURE);
			}
			if (len < MAXBUF)
			{
				printf(GREEN"\t[+]receive %s complete!\n"RESET, curr->data);
				break;
			}
		}
		close(inf->fd);
		fn_setTimes(curr->data, curr->time);
		curr=curr->next;
	}
}

/*
#	Функция для отправки файла с сервера на клиент
*/
static void	fn_sendingFiles(t_listRec *curr, t_info *inf)
{
	int		len;

	while(curr)
	{
		inf->fd = open(curr->data, O_RDONLY);
		if(inf->fd < 0)
		{
			perror(curr->data);
			continue;
		}
		SSL_write(inf->ssl, "rec", 4);
		SSL_write(inf->ssl, curr->data, strlen(curr->data));
		printf(YELLOW"\t[!]sending %s started\n"RESET, curr->data);
		while (1)
		{
			bzero(inf->buff, MAXBUF);
			len = read(inf->fd, inf->buff, MAXBUF);
			if (len < 0)
			{
				perror("read");
				exit(1);
			}
			len = SSL_write(inf->ssl, inf->buff, len);
			if (len < MAXBUF)
			{
				SSL_write(inf->ssl, "ENDFILE", 8);
				printf(GREEN"\t[+]send file complete!\n"RESET);
				break;
			}
		}
		SSL_write(inf->ssl, "set_time", 9);
		SSL_write(inf->ssl, curr->data, strlen(curr->data));
		SSL_write(inf->ssl, curr->time, 15);
		close(inf->fd);
		curr=curr->next;
	}
}

/*
#	Функция для получения директории с клиента на сервер
*/
static void	fn_receiveDir(t_listRec *curr, t_info *inf, char* c)
{
	while (curr)
	{
		mkdir(curr->data, 0755);
		chdir(curr->data);
		SSL_write(inf->ssl, "cd", 3);
		SSL_write(inf->ssl, curr->data, strlen(curr->data));
		printf(YELLOW"\t[!]recording DIR %s started\n"RESET, curr->data);
		fn_solver(c, inf);
		SSL_write(inf->ssl, "cd", 3);
		SSL_write(inf->ssl, "..", 3);
		chdir("..");
		SSL_write(inf->ssl, "set_time", 9);
		SSL_write(inf->ssl, curr->data, strlen(curr->data));
		SSL_write(inf->ssl, curr->time, 15);
		fn_setTimes(curr->data, curr->time);
		printf(GREEN"\t[+]receive DIR %s complete!\n"RESET, curr->data);
		curr = curr->next;
	}
}

/*
#	Функция для отправки директории с сервера на клиент
*/
static void	fn_sendDir(t_listRec *curr, t_info *inf, char* c)
{
	while (curr)
	{
		SSL_write(inf->ssl, "mkdir && cd", 12);
		SSL_write(inf->ssl, curr->data, strlen(curr->data));
		chdir(curr->data);
		printf(YELLOW"\t[!]sending DIR %s started\n"RESET, curr->data);
		fn_solver(c, inf);
		SSL_write(inf->ssl, "cd", 3);
		SSL_write(inf->ssl, "..", 3);
		chdir("..");
		SSL_write(inf->ssl, "set_time", 9);
		SSL_write(inf->ssl, curr->data, strlen(curr->data));
		SSL_write(inf->ssl, curr->time, 15);
		fn_setTimes(curr->data, curr->time);
		printf(GREEN"\t[+]send DIR complete!\n"RESET);
		curr = curr->next;
	}
}

/*
#	Функция для отправки и вывода информации о конфликтующих файлах
*/
static void	fn_confilctFile(t_listRec *curr, t_info *inf)
{
	while(curr)
	{
		SSL_write(inf->ssl, "err", 4);
		SSL_write(inf->ssl, curr->data, strlen(curr->data));
		printf(YELLOW"\t[!]CONFLICT FILES: %s\n"RESET, curr->data);
		curr= curr->next;
	}
}

/*
#	Функция для выполнения синхронизации
*/
static void	fn_solver(char *c, t_info *inf)
{
	t_listsRecResp	test;

	fn_writeListDir();
	fn_receive_c_file(inf);
	fn_initListRecResp(&test);
	fn_timingModes(&test, c);

	fn_receivingFiles(test.freadRec, inf);
	fn_sendingFiles(test.fwriteRec, inf);
	fn_receiveDir(test.dreadRec, inf, c);
	fn_sendDir(test.dwriteRec, inf, c);
	fn_confilctFile(test.conflict, inf);
	remove(".s_files");
	remove(".c_files");
	fn_removeListsRec(&test);
}

/*
#	Функция для выполнения соединения с клиентом и последующей синхронизации
*/
void	fn_connetion(t_info *inf)
{
	int		acc;
	char	sync_mode;

	SSL_library_init();//Регистрирует доступные шифры и дайджесты SSL/TLS. 
	fn_initServerCTX(inf);
	fn_loadCertificates(inf, "./src/CA/server.crt",
						"./src/CA/server.key",
						"./src/CA/ca.crt");
	fn_openListener(inf);
	chdir(inf->s_dir);
	while(1)
	{
		inf->sock_len = sizeof(struct sockaddr);
		if ((inf->fd_client = accept(inf->fd_serv,
									(struct sockaddr *)&(inf->client_addr),
									 &(inf->sock_len))) == -1)
		{
			perror(RED"\t[-]Accept");
			continue;
		}
		printf(GREEN"\t[+]Accept\n"RESET);
		if (!(inf->ssl = SSL_new(inf->ctx)))
		{
			perror(RED"\t[-]Could not get an SSL handle from the context");
			close(inf->fd_client);
			continue;
		}
		printf(GREEN"\t[+]SSL structure created\n"RESET);
		if (!SSL_set_fd(inf->ssl, inf->fd_client))
		{
			perror(RED"\t[-]Failed to bind fd and SSL");
			SSL_free(inf->ssl);
			close(inf->fd_client);
		}
		if ((acc = SSL_accept(inf->ssl)) != 1)
		{
			perror(RED"\t[-]Could not perform SSL handshake");
			if (acc != 0)
				SSL_shutdown(inf->ssl);
			SSL_free(inf->ssl);
			close(inf->fd_client);
			continue;
		}
		printf(GREEN"\t[+]SSL handshake successful with %s:%d\n"RESET,
 				inet_ntoa(inf->client_addr.sin_addr),
				ntohs(inf->client_addr.sin_port));
		sync_mode = '0';
		while (1)
		{
			SSL_read(inf->ssl, &sync_mode, sizeof(char));
			if (sync_mode == '0')
				continue;
			else if (sync_mode == '4' || sync_mode == '5')
				break;
			fn_GetDirPhone(inf);
			fn_solver(&sync_mode, inf);
			SSL_write(inf->ssl, "bye", 4);
			chdir(inf->s_dir);
		}
		SSL_shutdown(inf->ssl);
		SSL_free(inf->ssl);
		close(inf->fd_client);
		if (sync_mode == '5')
		{
			printf(GREEN"\t[+]Client disconnected, shutdown\n"RESET);
			break;
		}
		printf(GREEN"\t[+]The client is disconnected, idle\n"RESET);
	}
	close(inf->fd_serv);
	SSL_CTX_free(inf->ctx);
}