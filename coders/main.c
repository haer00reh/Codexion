#include "Codexion.h"


bool	arg_to_long(char *str, long *ret)
{
	*ret = ft_atol(str);

	if (*ret < 0)
	{
		fprintf(stderr, "Error occured while checking, found a negative number\nPlease provide a positive number\n\nquitting...\n");
		return false;
	}
	else
	return true;
}


int main(int ac, char **av)
{
	long	(number_of_coders), (number_of_compiles_required), (time_to_burnout), (time_to_compile), (time_to_debug), (time_to_refactor), (dongle_cooldown);
	char *schedule;


	if (!(arg_checker(av)))
		return -1;
	else if (!(arg_to_long(av[1], &number_of_coders)))
		return -1;
	else if (!(arg_to_long(av[2], &number_of_compiles_required)))
		return -1;
	else if (!(arg_to_long(av[3], &time_to_burnout)))
		return -1;
	else if (!(arg_to_long(av[4], &time_to_compile)))
		return -1;
	else if (!(arg_to_long(av[5], &time_to_debug)))
		return -1;
	else if (!(arg_to_long(av[6], &time_to_refactor)))
		return -1;
	else if (!(arg_to_long(av[7], &dongle_cooldown)))
		return -1;
}
