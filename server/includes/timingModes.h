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
	char	**data; //Массив строк, хранящий name, time и поле DIR, если файл -- каталог
	t_list	*next; // Указатель на следующий элемент списка
	t_list	*prev; // Указатель на предыдущий элемент списка
};
typedef struct s_fileConversion
{
	t_list	*s_head; //Указатель на первый элемент списка файлов сервера
	t_list	*c_head; //Указатель на первый элемент списка файлов клиента
	int		s_size; //Количество элементов в списке сервера
	int		c_size; //Количество элементов в списке клиента
}	t_fileConversion;

void	fn_timingModes(t_listsRecResp *recLst, char *s);

#endif