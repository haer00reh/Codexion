/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_cycle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:00:00 by haer-reh          #+#    #+#             */
/*   Updated: 2026/05/01 16:22:33 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

void	join_coder_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
}

bool	simulation_stopped(t_simulation *sim)
{
	bool	stopped;

	pthread_mutex_lock(&sim->stop_mutex);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->stop_mutex);
	return (stopped);
}

void	sleep_ms(long ms, t_simulation *sim)
{
	long	start;

	start = get_timestamp_ms();
	while (get_timestamp_ms() - start < ms)
	{
		if (simulation_stopped(sim))
			break ;
		usleep(500);
	}
}

bool	take_dongles(t_coder *coder, t_dongle *first, t_dongle *second)
{
	if (!acquire_dongle(coder, first))
		return (false);
	if (second != first)
	{
		if (!acquire_dongle(coder, second))
			return (release_dongle(coder, first), false);
		if (!simulation_stopped(coder->sim))
			print_coder_state(coder, "has taken a dongle");
		if (!simulation_stopped(coder->sim))
			print_coder_state(coder, "has taken a dongle");
	}
	return (true);
}

bool	run_coder_cycle(t_coder *coder, t_dongle *first, t_dongle *second)
{
	if (!take_dongles(coder, first, second))
		return (false);
	if (!simulation_stopped(coder->sim))
	{
		pthread_mutex_lock(&coder->sim->counter_mutex);
		coder->last_compile_start = get_timestamp_ms();
		pthread_mutex_unlock(&coder->sim->counter_mutex);
		if (!simulation_stopped(coder->sim))
			print_coder_state(coder, "is compiling");
		sleep_ms(coder->sim->time_to_compile, coder->sim);
	}
	release_dongle(coder, first);
	if (second != first)
		release_dongle(coder, second);
	if (!simulation_stopped(coder->sim))
		(print_coder_state(coder, "is debugging"), sleep_ms(
				coder->sim->time_to_debug, coder->sim));
	if (!simulation_stopped(coder->sim))
		(print_coder_state(coder, "is refactoring"), sleep_ms(
				coder->sim->time_to_refactor, coder->sim));
	pthread_mutex_lock(&coder->sim->counter_mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->sim->counter_mutex);
	return (true);
}
