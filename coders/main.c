/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 17:30:03 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/01 17:30:03 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

static bool	start_coder_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL, runtime_coder_routine,
				&sim->coders[i]) != 0)
		{
			while (--i >= 0)
				pthread_join(sim->coders[i].thread, NULL);
			return (false);
		}
		i++;
	}
	return (true);
}

static void	join_coder_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

static void	init_compile_starts(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->simulation_start_time;
		i++;
	}
}

static int	run_simulation(t_simulation *sim)
{
	if (!start_coder_threads(sim))
	{
		destroy_simulation_runtime(sim);
		return (-1);
	}
	pthread_create(&sim->monitor_thread, NULL, burn_out_monitor, sim);
	join_coder_threads(sim);
	pthread_join(sim->monitor_thread, NULL);
	destroy_simulation_runtime(sim);
	return (0);
}

int	main(int ac, char **av)
{
	t_simulation	sim;

	(void)ac;
	memset(&sim, 0, sizeof(sim));
	if (!(arg_checker(av)))
		return (-1);
	if (!init_simulation_from_args(&sim, av))
		return (-1);
	sim.simulation_start_time = get_timestamp_ms();
	init_compile_starts(&sim);
	return (run_simulation(&sim));
}
