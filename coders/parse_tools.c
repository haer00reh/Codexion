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

void	destroy_simulation_runtime(t_simulation *sim)
{
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->counter_mutex);
}

bool init_dongles(t_simulation *sim)
{
	int i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
	{
		free(sim->coders);
		return (false);
	}
	i = 0;

	while (i < sim->number_of_coders)
	{
		heap_init(&sim->dongles[i].waiting_heap, sim->number_of_coders);
		sim->dongles[i].id = i + 1;
		sim->dongles[i].in_use = false;
		sim->dongles[i].available_at = 0;
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		i++;
	}
	return (true);
}

bool init_coder(t_simulation *sim)
{
	int i;

	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
		return (false);
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].thread = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1) % sim->number_of_coders];
		sim->coders[i].sim = sim;
		i++;
	}
	return (true);
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
	if (!init_dongles(sim))
		return (destroy_simulation_runtime(sim), false);
	if (!init_coder(sim))
		return (destroy_simulation_runtime(sim), false);
	if (strcmp(av[8], "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(av[8], "edf") == 0)
		sim->scheduler = EDF;
	else
		return (destroy_simulation_runtime(sim), false);

	return (true);
}


long	ft_atol(char *str)
{
	long (sign), (nbr);
	sign = 1;
	nbr = 0;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	if (*str < '0' || *str > '9')
		return 0;
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return 0;
		nbr = (nbr * 10) + (*str - '0');
		if ((sign == 1 && nbr > 2147483647)
			|| (sign == -1 && nbr > 2147483648))
			return 0;
		str++;
	}
	return (nbr * sign);
}
