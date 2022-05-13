#include "../../includes/utils.h"

/*
#	Функция для очистки указателя на указатель (массив строк)
*/
void	fn_cleanDict(char **dict)
{
	int i;

	i = -1;
	while(dict[++i])
		free(dict[i]);
	free(dict);
}

/*
#	Функция для поиска подстроки (needle) в строке (haystack) до len символов.
#	Возвращает указатель на первое вхождение, либо NULL, если таковое не найдено.
*/
char	*fn_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	count;

	if (!*needle)
		return ((char *)haystack);
	if (!len || !(*haystack))
		return ((void *)0);
	i = 0;
	count = 0;
	while (i + count <= len && haystack[count])
	{
		i = 0;
		while (needle[i] && needle[i] == haystack[count + i]
			&& i + count < len)
			i++;
		if (needle[i] == '\0')
			return (&((char *)haystack)[count]);
		if (i > 1)
			count += i - 1;
		else
			count++;
	}
	return ((void *)0);
}

/*
#	Функция для проверки существования дирректории (path).
#	Возврещает 0 в случае успеха, иначе -1.
*/
int	fn_existDir(char *path)
{
	DIR *dirptr;

	if ((dirptr = opendir(path)) != NULL)
	{
		closedir(dirptr);
		return(0);
	}
	return (-1);
}