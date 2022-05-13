#include "../includes/steps2hell.h"

int main(void)
{
	t_info	inf;

	fn_checkStartDir(&inf);
	fn_connetion(&inf);
	free(inf.s_dir);
	return (0);
}