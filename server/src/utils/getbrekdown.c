#include "../../includes/utils.h"

static char	*fn_strrchr(const char *string, int c)
{
	char	*str_tmp;

	if (c == '\0')
	{
		while (*string)
			string++;
		return ((char *)string);
	}
	if (!(*string))
		return ((void *)0);
	str_tmp = (void *)0;
	while (*string)
	{
		if (*string == (char)c)
			str_tmp = (char *)string;
		string++;
	}
	if (str_tmp == (void *)0)
		return ((void *)0);
	return (str_tmp);
}

static int	fn_cnt_word(char *s)
{
	int		pt;
	char	*ret;

	ret = fn_strrchr(s,' ');
	if (ret != (void *)0)
	{
		pt = ret - s;
		if (s[pt + 1] >= '0' && s[pt + 1] <= '9')
			return (2);
		else if (s[pt + 1] == 'D')
			return (3);
	}
	return (0);
}

static int fn_len_name(char *s)
{
	int		pt;
	char	*ret;

	ret = fn_strrchr(s,' ');
	if (ret != (void *)0)
	{
		pt = ret - s;
		if (s[pt + 1] >= '0' && s[pt + 1] <= '9')
			return (pt);
		if (s[pt + 1] == 'D')
			return (pt - 15);
	}
	return (0);
}

static void	fn_strncpy(char *src, char *direc, int len)
{
	char	*tmp;
	
	tmp = src;
	if (len)
	{
		while(len - 1 > 0 && *tmp)
		{
			*direc++ = *tmp++;
			len--;
		}
		*direc = '\0';
	}
}

char	**fn_getBreakdown(char *s)
{
	char	**dict;
	int		cnt, len;

	cnt = fn_cnt_word(s);
	if (cnt != 0)
	{
		len = fn_len_name(s);
		dict = (char **)malloc(sizeof(char*) * cnt + 1);
		dict[0] = (char *) malloc(sizeof(char) * len + 1);
		dict[1] = (char *) malloc(sizeof(char) * 15);
		fn_strncpy(s, dict[0],len + 1);
		fn_strncpy(&s[len + 1], dict[1], 15);
		if (cnt == 3)
		{
			dict[2] = (char *) malloc(sizeof(char) * 4);
			fn_strncpy(&s[len + 16], dict[2], 4);
		}
		dict[cnt] = NULL;
		return (dict);
	}
	return (NULL);
}