#define _XOPEN_SOURCE
#include "../../includes/utils.h"
#include <time.h>
#include <utime.h>

/*
#	Функция устанавливает на файл или каталог время модификации
#	Входные параметры:
#	name -- имя файла или каталога на который будет установленно время.
#	times -- время, которое будет установленно в строковом представлении,
#		формат строки: YearMonthDayHourMinuteSecond.
*/
void	fn_setTimes(char *name, char *times)
{
	struct utimbuf	new_times;
	struct tm		pt_time = {0};
	time_t			ltmt;

	strptime(times, "%Y%m%d%H%M%S", &pt_time); //Функция парсит строку в соответсвии с заданным форматом в структуру tm
	ltmt = mktime(&pt_time); //Конвертируем struct tm в time_t
	new_times.modtime = ltmt; //В структуре описывающем время файла, в графе модификация время устанавливаем новое время
	if (utime(name, &new_times) != 0) //Изменяем время созданий (последней модификации) файла
		perror(name);
}