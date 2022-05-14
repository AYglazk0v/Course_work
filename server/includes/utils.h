#ifndef UTILS_H
# define UTILS_H

# include "steps2hell.h"

void	fn_cleanDict(char **dict);
char	*fn_strnstr(const char *haystack, const char *needle, size_t len);
int		fn_existDir(char *path);
char	**fn_split(char *s, char c);
char	**fn_getBreakdown(char *s);
void	fn_setTimes(char *name, char *times);

#endif