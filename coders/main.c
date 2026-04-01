#include "Codexion.h"


bool	arg_to_long(char *str, long *ret)
{
	*ret = atoi(str);

	if (*ret < 0)
	{
		fprintf(stderr, "Error occured while checking, found a negative number\nPlease provide a positive number\n\nquitting...\n");
		return false;
	}
	else
	return true;
}

void	destroy_simulation_runtime(t_simulation *sim)
{
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->counter_mutex);
}

bool	init_simulation_from_args(t_simulation *sim, char **av)
{
	int	ret;

	sim->coders = NULL;
	sim->dongles = NULL;
	sim->monitor_thread = 0;
	sim->simulation_start_time = 0;
	sim->stop = false;
	sim->fifo_sequence = 0;
	ret = pthread_mutex_init(&sim->print_mutex, NULL);
	if (ret != 0)
		return (false);
	ret = pthread_mutex_init(&sim->stop_mutex, NULL);
	if (ret != 0)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		return (false);
	}
	ret = pthread_mutex_init(&sim->counter_mutex, NULL);
	if (ret != 0)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (false);
	}
	if (!arg_to_long(av[1], &sim->number_of_coders))
		return (destroy_simulation_runtime(sim), false);
	if (!arg_to_long(av[2], &sim->number_of_compiles_required))
		return (destroy_simulation_runtime(sim), false);
	if (!arg_to_long(av[3], &sim->time_to_burnout))
		return (destroy_simulation_runtime(sim), false);
	if (!arg_to_long(av[4], &sim->time_to_compile))
		return (destroy_simulation_runtime(sim), false);
	if (!arg_to_long(av[5], &sim->time_to_debug))
		return (destroy_simulation_runtime(sim), false);
	if (!arg_to_long(av[6], &sim->time_to_refactor))
		return (destroy_simulation_runtime(sim), false);
	if (!arg_to_long(av[7], &sim->dongle_cooldown))
		return (destroy_simulation_runtime(sim), false);
	sim->schedule = av[8];
	return (true);
}


int main(int ac, char **av)
{
	t_simulation	sim;

	(void)ac;
	memset(&sim, 0, sizeof(sim));

	if (!(arg_checker(av)))
		return -1;
	if (!init_simulation_from_args(&sim, av))
		return -1;
	destroy_simulation_runtime(&sim);
	return (0);
}
