#include "../../includes/get_next_line_bonus.h"

/*
#	Функция для очистки указателя на указатель (массив строк) в случае ошибки выделения памяти
#	Пробрасывает NULL в вызываемую функцию.
*/
static char	**fn_clear(char **tab)
{
	unsigned int	i;

	i = 0;
	while (tab[i])
		free(tab[i++]);
	free(tab);
	return ((void *)0);
}

/*
#	Функция для подсчета символов в строке (str) до следующего разделителя (sep).
#	Возвращает количество символов.
*/
static size_t	fn_strlen2sep(const char *str, char sep)
{
	size_t			i;

	i = 0;
	while (str[i] && str[i] != sep)
		i++;
	return (i);
}

/*
#	Функция для подсчета "слов" в строке (str) с условием разделения по сепаратору (sep).
#	Возвращает количество "слов".
*/
static size_t	fn_count_words(const char *str, char sep)
{
	size_t			i;
	size_t			count;

	i = 0;
	count = 0;
	while (str[i])
	{
		if ((str[i] != sep && str[i + 1] && str[i + 1] == sep)
			|| (str[i] != sep && !str[i + 1]))
			count++;
		i++;
	}
	return (count);
}

/*
#	Функция разбивает строку (s) по сепаратору (c) и возрващает указатель на указатель
#	(массив строк), оканчивающийся NULL ( (void*)0 )
*/
char	**fn_split(char *s, char c)
{
	size_t	i;
	size_t	start;
	char	**dict;

	i = 0;
	start = 0;
	if (!s)
		return ((void *)0);
	dict = malloc((fn_count_words(s, c) + 1) * sizeof(char *));
	if (dict == NULL)
		return ((void *)0);
	while (s[i++] == c)
		start++;
	i = -1;
	while (++i < fn_count_words(s, c))
	{
		dict[i] = fn_substr(s, start, fn_strlen2sep(&s[start], c));
		if (dict[i] == NULL)
			return (fn_clear(dict));
		start += fn_strlen2sep(&s[start], c);
		while (s[start] == c)
			start++;
	}
	dict[i] = ((void *)0);
	return (dict);
}