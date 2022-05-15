#include "../../includes/utils.h"
#include "../../includes/get_next_line_bonus.h"

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

char	**fn_getBreakdown(char *s)
{
	char	**dict;
	int		cnt, len;

	if (!s)
		return ((void *)0);
	cnt = fn_cnt_word(s);
	dict = malloc(sizeof(char*) * (cnt + 1));
	if (cnt != 0)
	{
		len = fn_len_name(s);
		dict[0] = fn_substr(s,0,len);
		if (dict[0] == NULL)
		{
			fn_cleanDict(dict);
			return (NULL);
		}
		dict[1] = fn_substr(s,len+1, 14);
		if (dict[1] == NULL)
		{
			fn_cleanDict(dict);
			return (NULL);
		}
		if (cnt == 3)
		{
			dict[2] = fn_substr(s, len + 16, 4);
			if (dict[2] == NULL)
			{
				fn_cleanDict(dict);
				return (NULL);
			}
		}
	}
	dict[cnt] = ((void *)0);
	return (dict);
}