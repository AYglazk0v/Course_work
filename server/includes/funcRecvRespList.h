#ifndef FUCNRECVRESPLIST_H
# define FUCNRECVRESPLIST_H

# include <string.h>
# include "get_next_line_bonus.h"
# include <fcntl.h>
# include <stdio.h>

typedef struct s_listRec	t_listRec;

struct s_listRec
{
	char		*data;
	char		*time;
	t_listRec	*next;
	t_listRec	*prev;
};

typedef struct s_listsRecResp
{
	t_listRec	*fwriteRec; //перечень файлов, которые будут отправлены на телефон
	t_listRec	*freadRec; //перечень файлов, которые будут отправлены на сервер
	t_listRec	*dwriteRec; //перечень директорий, которые будут отправлены на телефон
	t_listRec	*dreadRec; //перечень директорий, которые будут отправлены на серве
	t_listRec	*conflict; //сонфликт файл-директория
}	t_listsRecResp;

void	fn_pushBackRec(t_listRec **head, char *date, char *time);
void	fn_removeListsRec(t_listsRecResp *env);
void	fn_initListRecResp(t_listsRecResp *env);

#endif