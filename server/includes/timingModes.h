#ifndef TIMINGMODES_H
# define TIMINGMODES_H

# include <string.h>
# include "get_next_line_bonus.h"
# include <fcntl.h>
# include <stdio.h>
# include "funcRecvRespList.h"

typedef struct s_list		t_list;

struct s_list
{
	char	**data;
	t_list	*next;
	t_list	*prev;
};

typedef struct s_fileConversion
{
	t_list	*s_head;
	t_list	*c_head;
	int		s_size;
	int		c_size;
}	t_fileConversion;

void	fn_timingModes(t_listsRecResp *recLst, char *s);

#endif