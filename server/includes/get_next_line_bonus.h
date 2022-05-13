#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024	
# endif

# ifndef MAX_FD
#  define MAX_FD 1024	
# endif

# include <fcntl.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

size_t		fn_strlen(char *s);
char		*fn_strchr(char *string, int symbol);
char		*get_next_line(int fd);
char		*fn_substr(char *s, unsigned int start, size_t len);

#endif
