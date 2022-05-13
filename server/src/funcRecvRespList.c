#include "../includes/funcRecvRespList.h"
#include "../includes/timingModes.h"
#include "../includes/listOfFiles.h"

/*
#	Функция создает дубликат строки (str) с условием выделения памяти.
#	Возвращает указатель на новую строке, либо NULL в случае неудачи.
*/
static char	*fn_strdup(const char *str)
{
	char	*cpy;
	int		i;
	int		size;

	i = 0;
	size = 0;
	size = strlen(str);
	cpy = (char *)malloc(sizeof(char) * (size + 1));
	if (cpy == NULL)
		return ((void *)0);
	while (size--)
	{
		cpy[i] = str[i];
		i++;
	}
	cpy[i] = '\0';
	return (cpy);
}

/*
#	Функция для получения последнего экзмепляра t_listRec в списке (head).
#	Возвращает указатель на последний t_listRec, если существует head, иначе NULL.
*/
static t_listRec	*fn_getLast(t_listRec *head)
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
static void	fn_delElem(t_listRec **head, t_listRec *delElem)
{
	if (*head == NULL || delElem == NULL)
		return;
	if (*head == delElem)
		*head = delElem->next;
	if (delElem->next != NULL)
		delElem->next->prev= delElem->prev;
	if (delElem->prev != NULL)
		delElem->prev->next=delElem->next;
	free(delElem->data);
	free(delElem->time);
	free(delElem);
}

/*
#	Функция для инициализации структуры t_listsRecResp.
*/
void	fn_initListRecResp(t_listsRecResp *env)
{
	env->conflict = NULL;
	env->dreadRec = NULL;
	env->dwriteRec = NULL;
	env->freadRec = NULL;
	env->fwriteRec = NULL;
}

/*
#	Функция для очистки стркутуры t_listsRecResp
*/
void	fn_removeListsRec(t_listsRecResp *env)
{
	while (env->conflict)
		fn_delElem(&env->conflict, env->conflict);
	while (env->dreadRec)
		fn_delElem(&env->dreadRec, env->dreadRec);
	while (env->dwriteRec)
		fn_delElem(&env->dwriteRec, env->dwriteRec);
	while (env->freadRec)
		fn_delElem(&env->freadRec, env->freadRec);
	while (env->fwriteRec)
		fn_delElem(&env->fwriteRec, env->fwriteRec);
}

/*
#	Функция для создания нового экзмепляра t_listRec в конец списка (head)
#	и заполнения его полей {date, time}.
#	В случае ошибки выделения памяти под новый элемент вызывается ошибка и программа завершается.
*/
void	fn_pushBackRec(t_listRec **head, char *date, char *time)
{
	t_listRec	*tmp;
	t_listRec	*last;

	last = fn_getLast(*head);
	tmp = (t_listRec *) malloc(sizeof(t_listRec));
	if (!tmp)
	{
		//TODO ОБРАБОТКА ОШИБКИ ПАМЯТИ (ОЧИСТИТЬ?!)
		perror("Err malloc (PB)");
		exit(EXIT_FAILURE);
	}
	tmp->data = fn_strdup(date);
	tmp->time = fn_strdup(time);
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

