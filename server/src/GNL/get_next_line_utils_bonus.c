#include "../../includes/get_next_line_bonus.h"

size_t	fn_strlen(char *s)
{
	size_t	i;

	i = 0;
	while (*(s + i))
		i++;
	return (i);
}

char	*fn_strchr(char *string, int symbol)
{
	if (!string)
		return (NULL);
	while (*string)
	{
		if (*string == (char)symbol)
			return (string);
		string++;
	}
	if (*string == (char)symbol)
		return (string);
	return (NULL);
}

char	*fn_substr(char *s, unsigned int start, size_t len)
{
	char			*sub;
	unsigned int	i;

	i = 0;
	if (!s)
		return ((void *)0);
	if ((unsigned int)fn_strlen(s) < start)
	{
		sub = malloc(sizeof(char) * 1);
		sub[0] = '\0';
		return (sub);
	}
	if ((unsigned int)fn_strlen(s) - start < len)
		len = (unsigned int)fn_strlen(s) - start;
	sub = malloc(sizeof(char) * (len + 1));
	if (sub == NULL)
		return ((void *)0);
	while (i < len)
	{
		sub[i] = s[start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}