/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:00:00 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/26 10:00:00 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

static bool	coder_burned_out(t_simulation *sim, int i, long *current, int *id)
{
	long	last_compile_start;

	*current = get_timestamp_ms();
	pthread_mutex_lock(&sim->counter_mutex);
	last_compile_start = sim->coders[i].last_compile_start;
	*id = sim->coders[i].id;
	pthread_mutex_unlock(&sim->counter_mutex);
	if (sim->time_to_burnout <= 0)
		return (false);
	if (*current - last_compile_start >= sim->time_to_burnout)
		return (true);
	return (false);
}

static void	wake_waiting_coders(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}

static int	check_simulation_state(t_simulation *sim, long *current,
		int *coder_id)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (coder_burned_out(sim, i, current, coder_id))
			return (1);
		i++;
	}
	pthread_mutex_lock(&sim->counter_mutex);
	i = 0;
	while (i < sim->number_of_coders)
	{
		if (sim->coders[i].compiles_done < sim->number_of_compiles_required)
			return (pthread_mutex_unlock(&sim->counter_mutex), 0);
		i++;
	}
	pthread_mutex_unlock(&sim->counter_mutex);
	return (2);
}

static void	finish_simulation(t_simulation *sim, int state, long current,
		int coder_id)
{
	if (state == 1)
	{
		pthread_mutex_lock(&sim->print_mutex);
		printf("%ld %d is burned out\n", current - sim->simulation_start_time,
			coder_id);
		pthread_mutex_unlock(&sim->print_mutex);
		wake_waiting_coders(sim);
	}
	pthread_mutex_lock(&sim->stop_mutex);
	sim->stop = true;
	pthread_mutex_unlock(&sim->stop_mutex);
}

void	*burn_out_monitor(void *arg)
{
	t_simulation	*sim;
	long			current;
	int				coder_id;
	int				state;

	sim = (t_simulation *)arg;
	while (1)
	{
		state = check_simulation_state(sim, &current, &coder_id);
		if (state == 1 || state == 2)
		{
			finish_simulation(sim, state, current, coder_id);
			return (NULL);
		}
		usleep(10);
	}
}
