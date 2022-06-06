#ifndef STEPS2HELL_H
# define STEPS2HELL_H

# ifndef MAXBUF
#  define MAXBUF 4096
# endif

# define PORT 3389

# define RESET	"\033[0m"
# define RED	"\033[1;31m"
// # define YELLOW	"\033[1;31m"
# define YELLOW	"\033[1;33m"
# define GREEN	"\033[1;32m"

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <errno.h>
# include <dirent.h>

# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/wait.h>
# include <arpa/inet.h>
# include <openssl/ssl.h>
# include <openssl/err.h>
# include <ifaddrs.h>
# include <netdb.h>

# include "utils.h"

typedef struct s_info
{
	char				*s_dir;	//Строка хранящая стартовую директорию
	char				buff[MAXBUF]; //Буфер для работы с файлами
	int					fd_serv, fd_client, fd; // Файловые дескрипторы
	socklen_t			sock_len; //Беззнаковый целочисленный тип длины не менее 32 бит. 
	SSL_CTX				*ctx; //Контекст (настройка параметров) для TLS соединения.
	SSL					*ssl; //Объект типа ssl
	struct	sockaddr_in	server_addr, client_addr;//Адрес и порт сервера и клиента соответсвенно.
}	t_info;

void	fn_checkStartDir(t_info *inf);
void	fn_connetion(t_info *inf);

#endif