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

static void	wait_dongle_until_ready(t_dongle *dongle)
{
	struct timespec	wake_at;
	long			now;
	long			remaining;

	now = get_timestamp_ms();
	if (now >= dongle->available_at)
	{
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
		return ;
	}
	remaining = dongle->available_at - now;
	clock_gettime(CLOCK_REALTIME, &wake_at);
	wake_at.tv_sec += remaining / 1000;
	wake_at.tv_nsec += (remaining % 1000) * 1000000;
	if (wake_at.tv_nsec >= 1000000000)
	{
		wake_at.tv_sec++;
		wake_at.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &wake_at);
}

static bool	start_coder_threads(t_simulation *sim)
{
	int	i;

	i = 0;
	while (i < sim->number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				runtime_coder_routine, &sim->coders[i]) != 0)
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

bool acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	request_submission(coder->sim, coder, dongle);
	while (true)
	{
		if (can_take_dongle(coder, dongle))
		{
			dongle->in_use = true;
			heap_pop_min(&dongle->waiting_heap);
			
			pthread_mutex_unlock(&dongle->mutex);
			return (true);
		}
		wait_dongle_until_ready(dongle);
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (false);
}

bool can_take_dongle(t_coder *coder, t_dongle *dongle)
{
	if (dongle->waiting_heap.size == 0)
		return (false);
	else if (dongle->in_use)
		return (false);
	else if (get_timestamp_ms() < dongle->available_at)
		return (false);
	else if (!heap_peek(coder, dongle))
		return (false);
	return (true);
}

void release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = false;
	dongle->available_at = get_timestamp_ms() + coder->sim->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
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
	sim.simulation_start_time = get_timestamp_ms();
	if (!start_coder_threads(&sim))
	{
		destroy_simulation_runtime(&sim);
		return (-1);
	}
	join_coder_threads(&sim);

	destroy_simulation_runtime(&sim);
	return (0);
}
