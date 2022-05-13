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
	int				fd;
	struct dirent	**namelist;
	char			t_buf[15];
	struct tm		*tm;
	struct stat		times;
}t_find_f;

void	fn_writeListDir(void);

#endif