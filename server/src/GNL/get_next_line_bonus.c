#include "../../includes/get_next_line_bonus.h"

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
	join = (char *)malloc(sizeof(char) * (fn_strlen(s1) + fn_strlen(s2) + 1));
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

static char	*fn_read_file(int fd, char *balance_line)
{
	char	*buff;
	int		size_read;

	buff = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buff)
		return (NULL);
	size_read = 1;
	while (!fn_strchr(balance_line, '\n') && size_read)
	{
		size_read = read(fd, buff, BUFFER_SIZE);
		if (size_read == -1)
		{
			free(buff);
			return (NULL);
		}
		buff[size_read] = '\0';
		balance_line = fn_strjoin(balance_line, buff);
	}
	free(buff);
	return (balance_line);
}

static char	*fn_get2n(char *balance_line)
{
	char	*line;
	size_t	i;

	i = 0;
	if (!balance_line[i])
		return (NULL);
	while (balance_line[i] && balance_line[i] != '\n')
		i++;
	line = (char *)malloc(sizeof(char) * i + 2);
	if (!line)
		return (NULL);
	i = 0;
	while (balance_line[i] && balance_line[i] != '\n')
	{
		line[i] = balance_line[i];
		i++;
	}
	if (balance_line[i] == '\n')
	{
		line[i] = balance_line[i];
		i++;
	}
	line[i] = '\0';
	return (line);
}

static char	*fn_get_nbalance(char *balance_line)
{
	char	*nbalance;
	size_t	i;
	size_t	j;

	i = 0;
	while (balance_line[i] && balance_line[i] != '\n')
		i++;
	if (!balance_line[i])
	{
		free(balance_line);
		return (NULL);
	}
	nbalance = (char *)malloc(sizeof(char) * (fn_strlen(balance_line) - i + 1));
	if (!nbalance)
		return (NULL);
	i++;
	j = 0;
	while (balance_line[i])
		nbalance[j++] = balance_line[i++];
	nbalance[j] = '\0';
	free(balance_line);
	return (nbalance);
}

char	*get_next_line(int fd)
{
	char		*line;
	static char	*balance_line[MAX_FD];

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	balance_line[fd] = fn_read_file(fd, balance_line[fd]);
	if (!balance_line[fd])
		return (NULL);
	line = fn_get2n(balance_line[fd]);
	balance_line[fd] = fn_get_nbalance(balance_line[fd]);
	return (line);
}
