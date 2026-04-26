/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:00:00 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/26 10:00:00 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

void	destroy_simulation_runtime(t_simulation *sim)
{
	pthread_mutex_destroy(&sim->print_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_mutex_destroy(&sim->counter_mutex);
}

static bool	init_mutexes(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->print_mutex, NULL) != 0)
		return (false);
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
		return (pthread_mutex_destroy(&sim->print_mutex), false);
	if (pthread_mutex_init(&sim->counter_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->print_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (false);
	}
	return (true);
}

static bool	parse_numeric_values(t_simulation *sim, char **av)
{
	if (!arg_to_long(av[1], &sim->number_of_coders))
		return (false);
	if (!arg_to_long(av[2], &sim->number_of_compiles_required))
		return (false);
	if (!arg_to_long(av[3], &sim->time_to_burnout))
		return (false);
	if (!arg_to_long(av[4], &sim->time_to_compile))
		return (false);
	if (!arg_to_long(av[5], &sim->time_to_debug))
		return (false);
	if (!arg_to_long(av[6], &sim->time_to_refactor))
		return (false);
	if (!arg_to_long(av[7], &sim->dongle_cooldown))
		return (false);
	return (true);
}

static bool	set_scheduler(t_simulation *sim, char *scheduler)
{
	if (strcmp(scheduler, "fifo") == 0)
		sim->scheduler = FIFO;
	else if (strcmp(scheduler, "edf") == 0)
		sim->scheduler = EDF;
	else
		return (false);
	return (true);
}

bool	init_simulation_from_args(t_simulation *sim, char **av)
{
	sim->coders = NULL;
	sim->dongles = NULL;
	sim->monitor_thread = 0;
	sim->simulation_start_time = 0;
	sim->stop = false;
	sim->global_sequence = 0;
	if (!init_mutexes(sim))
		return (false);
	if (!parse_numeric_values(sim, av))
		return (destroy_simulation_runtime(sim), false);
	if (!init_dongles(sim))
		return (destroy_simulation_runtime(sim), false);
	if (!init_coder(sim))
		return (destroy_simulation_runtime(sim), false);
	if (!set_scheduler(sim, av[8]))
		return (destroy_simulation_runtime(sim), false);
	return (true);
}
