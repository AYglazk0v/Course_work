#include "../includes/timingModes.h"
#include "../includes/utils.h"

/*
#	Инициализация структуры t_fileConversion
*/
static void		fn_initList(t_fileConversion *env)
{
	env->c_head = NULL;
	env->s_head = NULL;
}

/*
#	Функция для получения последнего экзмепляра t_list в списке (head).
#	Возвращает указатель на последний t_list, если существует head, иначе NULL.
*/
static t_list	*fn_getLast(t_list *head)
{
	if (head == NULL)
		return (NULL);
	while (head->next)
		head = head->next;
	return (head);
}

/*
#	Функция для удаления элемента (delElem) в списке (head).
*/
static void		fn_delElem(t_list **head, t_list *delElem)
{
	if (*head == NULL || delElem == NULL)
		return;
	if (*head == delElem)
		*head = delElem->next;
	if (delElem->next != NULL)
		delElem->next->prev= delElem->prev;
	if (delElem->prev != NULL)
		delElem->prev->next=delElem->next;
	fn_cleanDict(delElem->data);
	free(delElem);
}

/*
#	Функция для создания нового экзмепляра t_list в конец списка (head)
#	и заполнения его полей {date, time}.
#	В случае ошибки выделения памяти под новый элемент вызывается ошибка и программа завершается.
*/
static void		fn_pushBack(t_list **head, char *date)
{
	t_list *tmp;
	t_list *last;

	last = fn_getLast(*head);
	tmp = (t_list *) malloc(sizeof(t_list));
	if (!tmp)
	{
		perror("Err malloc (PB)");
		exit(EXIT_FAILURE);
	}
	tmp->data = fn_getBreakdown(date);
	tmp->next = NULL;
	if (!last)
	{
		*head = tmp;
		(*head)->prev = NULL;
	}
	else
	{
		last->next = tmp;
		tmp->prev = last;
	}
}

/*
#	Функция для очистки стркутуры t_fileConversion
*/
static void		fn_removeLists(t_fileConversion *env)
{
	while (env->s_head)
		fn_delElem(&env->s_head, env->s_head);
	while (env->c_head)
		fn_delElem(&env->c_head, env->c_head);
}

/*
#	Функция для заполнения листа (head) данными из файла (src)
*/
static void		fn_strConversion(t_list **head, const char *src)
{
	int		fd;
	char	*line;
	
	fd = open(src, O_RDONLY);
	line = get_next_line(fd);
	while (line)
	{
		fn_pushBack(head, line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	free(line);
}

/*
#	Функция заполняет поля структуры (recv) согласно типу синхронизации:
#	Однонаправленная синхронизация данных с клиента на сервер.
*/
static void		fn_c2s(t_listsRecResp *recv)
{
	t_fileConversion	files;
	t_list				*curr_s;
	t_list				*curr_c;
	int					cmp1;
	int					cmp2;
	
	fn_initList(&files);
	fn_strConversion(&files.s_head, ".s_files");
	fn_strConversion(&files.c_head, ".c_files");
	curr_c = files.c_head;
	curr_s = files.s_head;
	while(curr_c && curr_s)
	{
		cmp1 = strcmp(curr_c->data[0], curr_s->data[0]);
		if (!cmp1)
		{
			cmp2 = strcmp(curr_c->data[1], curr_s->data[1]);
			if(cmp2 > 0)
			{
				if (curr_c->data[2] && curr_s->data[2])
					fn_pushBackRec(&recv->dreadRec, curr_c->data[0], curr_c->data[1]);
				else if (curr_c->data[2] || curr_s->data[2])
					fn_pushBackRec(&recv->conflict, curr_c->data[0], curr_c->data[1]);
				else
					fn_pushBackRec(&recv->freadRec, curr_c->data[0], curr_c->data[1]);
			}
			curr_c = curr_c->next;
			curr_s = curr_s->next;
		}
		else if (cmp1 > 0)
			curr_s = curr_s->next;
		else
		{
			if (curr_c->data[2])
				fn_pushBackRec(&recv->dreadRec, curr_c->data[0], curr_c->data[1]);
			else
				fn_pushBackRec(&recv->freadRec, curr_c->data[0], curr_c->data[1]);
			curr_c = curr_c ->next;
		}
	}
	while(curr_c)
	{
		if (curr_c->data[2])
			fn_pushBackRec(&recv->dreadRec, curr_c->data[0], curr_c->data[1]);
		else
			fn_pushBackRec(&recv->freadRec, curr_c->data[0], curr_c->data[1]);
		curr_c = curr_c ->next;
	}
	fn_removeLists(&files);
}

/*
#	Функция заполняет поля структуры (recv) согласно типу синхронизации:
#	Однонаправленная синхронизация данных с сервера на клиент.
*/
static void		fn_s2c(t_listsRecResp *recv)
{
	t_fileConversion	files;
	t_list				*curr_s;
	t_list				*curr_c;
	int					cmp1;
	int					cmp2;
	
	fn_initList(&files);
	fn_strConversion(&files.s_head, ".s_files");
	fn_strConversion(&files.c_head, ".c_files");
	curr_c = files.c_head;
	curr_s = files.s_head;
	while(curr_s && curr_c)
	{
		cmp1 = strcmp(curr_s->data[0], curr_c->data[0]);
		if (!cmp1)
		{
			cmp2 = strcmp(curr_s->data[1], curr_c->data[1]);
			if(cmp2 > 0)
			{
				if (curr_c->data[2] && curr_s->data[2])
					fn_pushBackRec(&recv->dwriteRec, curr_s->data[0], curr_s->data[1]);
				else if (curr_c->data[2] || curr_s->data[2])
					fn_pushBackRec(&recv->conflict, curr_s->data[0], curr_s->data[1]);
				else
					fn_pushBackRec(&recv->fwriteRec, curr_s->data[0], curr_s->data[1]);
			}
			curr_c = curr_c->next;
			curr_s = curr_s->next;
		}
		else if (cmp1 > 0)
			curr_c = curr_c->next;
		else
		{
			if (curr_s->data[2])
				fn_pushBackRec(&recv->dwriteRec, curr_s->data[0], curr_s->data[1]);
			else
				fn_pushBackRec(&recv->fwriteRec, curr_s->data[0], curr_s->data[1]);
			curr_s = curr_s ->next;
		}
	}
	while(curr_s)
	{
		if (curr_s->data[2])
			fn_pushBackRec(&recv->dwriteRec, curr_s->data[0], curr_s->data[1]);
		else
			fn_pushBackRec(&recv->fwriteRec, curr_s->data[0], curr_s->data[1]);
		curr_s = curr_s ->next;
	}
	fn_removeLists(&files);
}

/*
#	Функция заполняет поля структуры (recv) согласно типу синхронизации:
#	Двунаправленная синхронизация данных.
*/
static void		fn_bilateral(t_listsRecResp *recv)
{
	t_fileConversion	files;
	t_list					*curr_s;
	t_list					*curr_c;
	int						cmp1;
	int						cmp2;
	
	fn_initList(&files);
	fn_strConversion(&files.s_head, ".s_files");
	fn_strConversion(&files.c_head, ".c_files");
	curr_c = files.c_head;
	curr_s = files.s_head;
	while(curr_s && curr_c)
	{
		cmp1 = strcmp(curr_s->data[0], curr_c->data[0]);
		if (!cmp1)
		{
			cmp2 = strcmp(curr_s->data[1], curr_c->data[1]);
			if(cmp2 > 0)
			{
				if (curr_c->data[2] && curr_s->data[2])
					fn_pushBackRec(&recv->dwriteRec, curr_s->data[0], curr_s->data[1]);
				else if (curr_c->data[2] || curr_s->data[2])
					fn_pushBackRec(&recv->conflict, curr_s->data[0], curr_s->data[1]);
				else
					fn_pushBackRec(&recv->fwriteRec, curr_s->data[0], curr_s->data[1]);
			}
			else if (cmp2 < 0)
			{
				if (curr_c->data[2] && curr_s->data[2])
					fn_pushBackRec(&recv->dreadRec, curr_c->data[0], curr_c->data[1]);
				else if (curr_c->data[2] || curr_s->data[2])
					fn_pushBackRec(&recv->conflict, curr_c->data[0], curr_c->data[1]);
				else
					fn_pushBackRec(&recv->freadRec, curr_c->data[0], curr_c->data[1]);
			}
			curr_c = curr_c->next;
			curr_s = curr_s->next;
		}
		else if (cmp1 > 0)
		{
			if (curr_c->data[2])
				fn_pushBackRec(&recv->dreadRec, curr_c->data[0], curr_c->data[1]);
			else
				fn_pushBackRec(&recv->freadRec, curr_c->data[0], curr_c->data[1]);
			curr_c = curr_c ->next;
		}
		else
		{
			if (curr_s->data[2])
				fn_pushBackRec(&recv->dwriteRec, curr_s->data[0], curr_s->data[1]);
			else
				fn_pushBackRec(&recv->fwriteRec, curr_s->data[0], curr_s->data[1]);
			curr_s = curr_s ->next;
		}
	}
	while(curr_s)
	{
		if (curr_s->data[2])
			fn_pushBackRec(&recv->dwriteRec, curr_s->data[0], curr_s->data[1]);
		else
			fn_pushBackRec(&recv->fwriteRec, curr_s->data[0], curr_s->data[1]);
		curr_s = curr_s ->next;
	}
	while(curr_c)
	{
		if (curr_c->data[2])
			fn_pushBackRec(&recv->dreadRec, curr_c->data[0], curr_c->data[1]);
		else
			fn_pushBackRec(&recv->freadRec, curr_c->data[0], curr_c->data[1]);
		curr_c = curr_c ->next;
	}
	fn_removeLists(&files);
}

/*
#	Функция осуществяет заполнение стурктуры (recLst) в соответсвии с выбранным
#	на клиенте типом синхронизации (s)
*/
void			fn_timingModes(t_listsRecResp *recLst, char *s)
{
	if(!strncmp("1", s, 1))
		fn_bilateral(recLst);
	else if(!strncmp("2", s, 1))
		fn_s2c(recLst);
	else if(!strncmp("3", s, 1))
		fn_c2s(recLst);
	else
		exit(EXIT_FAILURE);
}