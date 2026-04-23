/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_and_coders.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 17:30:16 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/19 09:58:44 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"
void	print_coder_state(t_coder *coder, const char *state)
{
	long	timestamp;

	pthread_mutex_lock(&coder->sim->print_mutex);

	pthread_mutex_lock(&coder->sim->stop_mutex);
	if (coder->sim->stop)
	{
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		pthread_mutex_unlock(&coder->sim->print_mutex);
		return;
	}
	pthread_mutex_unlock(&coder->sim->stop_mutex);

	timestamp = get_timestamp_ms();
	printf("%ld %d %s\n",
		timestamp - coder->sim->simulation_start_time,
		coder->id,
		state);

	pthread_mutex_unlock(&coder->sim->print_mutex);
}

static void	sleep_ms(long ms, t_simulation *sim)
{
	long	start;

	start = get_timestamp_ms();
	while (get_timestamp_ms() - start < ms)
	{
		pthread_mutex_lock(&sim->stop_mutex);

		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			break;
		}
		pthread_mutex_unlock(&sim->stop_mutex);

		usleep(100);
	}
}

void request_submission(t_simulation *sim, t_coder *coder, t_dongle *dongle)
{
	long priority;

	if (sim->scheduler == FIFO)
		priority = 0;
	else
		priority = coder->last_compile_start + sim->time_to_burnout;

	pthread_mutex_lock(&sim->counter_mutex);
	heap_push(&dongle->waiting_heap, coder, priority, sim->global_sequence++);
	pthread_mutex_unlock(&sim->counter_mutex);
}



void *runtime_coder_routine(void *arg)
{
	t_coder			*coder;
	t_simulation	*sim;
	t_dongle		*first;
	t_dongle		*second;
	bool			has_first;
	bool			has_second;

	coder = (t_coder *)arg;
	sim = coder->sim;
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	coder->compiles_done = 0;

		if (sim->number_of_coders == 1)
		{
			if (!acquire_dongle(coder, first))
				return (NULL);
			if (!sim->stop)
				print_coder_state(coder, "has taken a dongle");

		return (NULL);
		}

	while (coder->compiles_done < sim->number_of_compiles_required)
	{
		pthread_mutex_lock(&sim->stop_mutex);
		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			break ;
		}
		pthread_mutex_unlock(&sim->stop_mutex);

		has_first = false;
		has_second = false;
		if (!acquire_dongle(coder, first))
			break;
		has_first = true;
		
		pthread_mutex_lock(&sim->stop_mutex);
		if (!sim->stop)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			print_coder_state(coder, "has taken a dongle");
		}

		if (second != first)
		{
			if (!acquire_dongle(coder, second))
			{
				if (has_first)
					release_dongle(coder, first);
				break ;
			}
			has_second = true;

		pthread_mutex_lock(&sim->stop_mutex);
		if (!sim->stop)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			print_coder_state(coder, "has taken a dongle");
		}

		}
		else
			has_second = true;

		pthread_mutex_lock(&sim->stop_mutex);
		if (!sim->stop && has_first && has_second)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			pthread_mutex_lock(&sim->read_write_mutex);
			coder->last_compile_start = get_timestamp_ms();
			pthread_mutex_unlock(&sim->read_write_mutex);
		pthread_mutex_lock(&sim->stop_mutex);
		if (!sim->stop)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			print_coder_state(coder, "is compiling");
			sleep_ms(sim->time_to_compile, sim);
		}
		}

		if (has_first)
			release_dongle(coder, first);
		if (second != first && has_second)
			release_dongle(coder, second);

		pthread_mutex_lock(&sim->stop_mutex);
		if (!sim->stop)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			print_coder_state(coder, "is debugging");
			sleep_ms(sim->time_to_debug, sim);
		}

		pthread_mutex_lock(&sim->stop_mutex);
		if (!sim->stop)
		{
			pthread_mutex_unlock(&sim->stop_mutex);
			print_coder_state(coder, "is refactoring");
			sleep_ms(sim->time_to_refactor, sim);
		}
		pthread_mutex_lock(&sim->read_write_mutex);
		coder->compiles_done++;
		pthread_mutex_unlock(&sim->read_write_mutex);
	}

	return (NULL);
}
