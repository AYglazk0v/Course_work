#ifndef LISTOFFILES_H
# define LISTOFFILES_H

# include <dirent.h> 
# include <string.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <time.h>
# include <unistd.h>
# include <stdio.h>
# include <errno.h>

# include "utils.h"

# define ABS(a) (a < 0) ? -a : a

typedef struct s_find_f
{
	int				fd;			//Файловый дескриптор для открытия файла ".s_files".
	struct dirent	**namelist;	//Структура хранящая перечень файлов в каталоге.
	char			t_buf[15];	//Строка для записи времени модификации.
	struct tm		*tm;		//Структура конвертации из stat в char* и послед. записи в файл.
	struct stat		times;		//Структура для получения информации о файле с помощью stat().
}t_find_f;

void	fn_writeListDir(void);

#endif