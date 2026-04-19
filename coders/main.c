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
	if (now < dongle->available_at)
	{
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
	else
	{
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
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

void *burn_out_monitor(void *arg)
{
	t_simulation	*sim;
	long			current_time;
	int				i;
	int				compiles;

	compiles = 0;
	sim = (t_simulation *)arg;
	while (1)
	{
		i = 0;
		while (i < sim->number_of_coders)
		{
			current_time = get_timestamp_ms();
			if (sim->time_to_burnout > 0 &&
			 current_time - sim->coders[i].last_compile_start >= sim->time_to_burnout)
			{
				pthread_mutex_lock(&sim->print_mutex);
				printf("%ld %d is burned out\n", current_time - sim->simulation_start_time, sim->coders[i].id);
				pthread_mutex_lock(&sim->stop_mutex);
				sim->stop = true;
				pthread_mutex_unlock(&sim->stop_mutex);
				pthread_mutex_unlock(&sim->print_mutex);

				i = 0;
				while (i < sim->number_of_coders)
				{
					pthread_mutex_lock(&sim->dongles[i].mutex);
					pthread_cond_broadcast(&sim->dongles[i].cond);
					pthread_mutex_unlock(&sim->dongles[i].mutex);
					i++;
				}
				return (NULL);
			}
			i++;
		}
		for (i = 0; i < sim->number_of_coders; i++)
		{
			if (sim->coders[i].compiles_done < sim->number_of_compiles_required)
				break;
		}
		if (i == sim->number_of_coders)
			pthread_mutex_lock(&sim->stop_mutex);
			sim->stop = true;
			pthread_mutex_unlock(&sim->stop_mutex);
		usleep(100);
	}
	return (NULL);
}

void start_monitor_thread(t_simulation *sim)
{
	pthread_create(&sim->monitor_thread, NULL, burn_out_monitor, sim);
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
		pthread_mutex_lock(&coder->sim->stop_mutex);
		if (coder->sim->stop)
		{
			pthread_mutex_unlock(&coder->sim->stop_mutex);
			pthread_mutex_unlock(&dongle->mutex);
			return (false);
		}
		pthread_mutex_unlock(&coder->sim->stop_mutex);

		if (can_take_dongle(coder, dongle))
		{
			dongle->in_use = true;
			heap_pop_min(&dongle->waiting_heap);
			pthread_mutex_unlock(&dongle->mutex);
			return (true);
		}
		wait_dongle_until_ready(dongle);
	}
}

bool can_take_dongle(t_coder *coder, t_dongle *dongle)
{
	if (dongle->waiting_heap.size == 0)
		return (false);
	if (dongle->in_use)
		return (false);
	if (get_timestamp_ms() < dongle->available_at)
		return (false);
	if (dongle->waiting_heap.arr[0].coder != coder)
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
		return (-1);
	if (!init_simulation_from_args(&sim, av))
		return (-1);

	sim.simulation_start_time = get_timestamp_ms();

	if (!start_coder_threads(&sim))
	{
		destroy_simulation_runtime(&sim);
		return (-1);
	}

	start_monitor_thread(&sim);

	join_coder_threads(&sim);
	pthread_join(sim.monitor_thread, NULL);

	destroy_simulation_runtime(&sim);
	return (0);
}
