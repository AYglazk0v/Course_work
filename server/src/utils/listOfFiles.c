#include "../../includes/listOfFiles.h"

/*
#	Функция создает временный файл .s_file содержащий перечень файлов
#	и директорий находящихся в текущем рабочем каталоге в алфавитном порядке.
#	Пример поля в файле (файл): steps2hell 200006220400
#	Пример поля в файле (файл): utils 199702211535 DIR
*/
void	fn_writeListDir(void)
{
	t_find_f	find;
	int			cnt_name;
	int			i;

	find.fd = open(".s_files", O_RDWR | O_CREAT | O_TRUNC, 00766);
	cnt_name = scandir(".", &find.namelist, 0, alphasort);
	if (cnt_name < 0)
	{
		perror("scandir");
		exit(EXIT_FAILURE);
	}
	else
	{
		i = -1;
		while (++i < cnt_name)
		{
			if(ABS(strcmp(find.namelist[i]->d_name, ".")
					&& strcmp(find.namelist[i]->d_name , "..")
					&& strcmp(find.namelist[i]->d_name , ".s_files")
					&& strcmp(find.namelist[i]->d_name , ".c_files")))
			{
				stat(find.namelist[i]->d_name, &find.times);
				find.tm = localtime(&find.times.st_mtime);
				strftime(find.t_buf, 15, "%Y%m%d%H%M%S", find.tm);
				write(find.fd, find.namelist[i]->d_name, strlen(find.namelist[i]->d_name));
				write(find.fd, " ", 1);
				write(find.fd, find.t_buf, 14);
				if (fn_existDir(find.namelist[i]->d_name) == 0)
					write(find.fd, " DIR", 4);
				write(find.fd, "\n", 1);
			}
			free(find.namelist[i]);
		}
		free(find.namelist);
	}
	close(find.fd);
}