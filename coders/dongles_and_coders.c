/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 17:30:16 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/01 17:30:17 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

void	print_coder_state(t_coder *coder, const char *state)
{
	long	timestamp;

	timestamp = get_timestamp_ms();
	pthread_mutex_lock(&coder->sim->print_mutex);
	printf("%ld %d %s\n",
		timestamp - coder->sim->simulation_start_time,
		coder->id,
		state);
	pthread_mutex_unlock(&coder->sim->print_mutex);
}

static void	sleep_ms(long ms)
{
	if (ms > 0)
		usleep(ms * 1000);
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
	while (!sim->stop && coder->compiles_done < sim->number_of_compiles_required)
	{
		if (!acquire_dongle(coder, first))
			break;
		print_coder_state(coder, "has taken a dongle");

		if (second != first)
		{
			if (!acquire_dongle(coder, second))
			{
				release_dongle(coder, first);
				break;
			}
			print_coder_state(coder, "has taken a dongle");
		}

		coder->last_compile_start = get_timestamp_ms();
		print_coder_state(coder, "is compiling");
		sleep_ms(sim->time_to_compile);

		release_dongle(coder, first);
		if (second != first)
			release_dongle(coder, second);

		coder->compiles_done++;

		print_coder_state(coder, "is debugging");
		sleep_ms(sim->time_to_debug);

		print_coder_state(coder, "is refactoring");
		sleep_ms(sim->time_to_refactor);
	}

	return (NULL);
}
