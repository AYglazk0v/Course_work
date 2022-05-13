#include "../../includes/steps2hell.h"

extern char **environ;

/*
#	Функция для склейки двух строк {s1,s2}.
#	Возвращает указатель на новую строку в случае успеха, иначе NULL.
*/
static char	*fn_strjoin(char *s1, char *s2)
{
	char	*join;
	size_t	i;
	size_t	j;

	if (!s1)
	{
		s1 = (char *)malloc(sizeof(char) * 1);
		s1[0] = '\0';
	}
	if (!(s1 && s2))
		return (NULL);
	join = (char *)malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
	if (!join)
		return (NULL);
	i = -1;
	while (s1[++i])
		join[i] = s1[i];
	j = 0;
	while (s2[j])
		join[i++] = s2[j++];
	join[i] = '\0';
	free(s1);
	return (join);
}

/*
#	Функция для получения домашней директории путем парсинга переменного окружения env.
#	Возрващает строку, если такая найдена, иначе программа завершается с выводом ошибки.
*/
static char	*fn_parsHome(char **env)
{
	int		i;
	int		j;
	char	*ret;

	i = 0;
	while (fn_strnstr(env[i], "HOME", 4) == 0)
		i++;
	if (!env[i])
	{
		perror(RED"\t[-]HOME was not found in env");
		exit(EXIT_FAILURE);
	}
	ret = (char *) malloc(sizeof(char) * strlen(env[i]) - 4);
	j = 0;
	while (env[i][5 + j])
	{
		ret[j] = env[i][5 + j];
		j++;
	}
	ret[j]='\0';
	return (ret);
}

/*
#	Функция для проверки существования каталога синхронизации (Steps2Hell) в домашней директории.
#	Если директориии не существует, то пользователю предлагается создать её.
#	Если в домашней директории существует одноименный файл, неявляющийся дирректорий, то выводится ошибка и программа завершается.
#	Иначе происходит модификация inf->s_dir: склеивается с "/Steps2Hell".
#	Возрващает строку, если такая найдена, иначе программа завершается с выводом ошибки.
*/
void	fn_checkStartDir(t_info *inf)
{
	int		status;
	char	c;

	inf->s_dir = fn_strjoin(fn_parsHome(environ), "/Steps2Hell");
	if (!inf->s_dir)
	{	
		perror(RED"\t[-]invalid dirName in fn_CheckDir");
		exit(EXIT_FAILURE);
	}
	status = access(inf->s_dir, F_OK);
	if (status < 0 && fn_existDir(inf->s_dir) < 0)
	{
		perror(RED"\t[!]failed to open folder");
		printf(GREEN"\t[?]Create a new directory [Y/N]? "RESET);
		scanf("%c", &c);
		if (c == 'y' || c == 'Y')
		{
			if(mkdir(inf->s_dir, 0775) == -1)
			{
				free(inf->s_dir);
				perror(RED"\t[-]Directory creation error");
				exit(EXIT_FAILURE);
			}
			printf(GREEN"\t[+]the directory ~/Steps2Hell the directory has been created\n"RESET);
		}
		else
		{
			free(inf->s_dir);
			exit(EXIT_FAILURE);
		}
	}
	else if (status == 0 && fn_existDir(inf->s_dir) < 0)
	{
		free(inf->s_dir);
		perror(RED"\t[-]The Steps2Hell file doesn't allow you to create a directory");
		exit(EXIT_FAILURE);
	}
}