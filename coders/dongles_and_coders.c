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

static void	print_coder_state(t_coder *coder, const char *state)
{
	long	timestamp;

	timestamp = get_timestamp_ms();
	pthread_mutex_lock(&coder->sim->print_mutex);
	printf("[%ld] coder %d %s\n",
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
	if (sim->scheduler == FIFO)
	{
		pthread_mutex_lock(&sim->counter_mutex);
		heap_push(&dongle->waiting_heap, coder, 0, sim->global_sequence++);
		pthread_mutex_unlock(&sim->counter_mutex);
	}
	else if (sim->scheduler == EDF)
	{
	long priority;

	priority = coder->last_compile_start + sim->time_to_burnout;
	pthread_mutex_lock(&sim->counter_mutex);
	heap_push(&dongle->waiting_heap, coder, priority, sim->global_sequence++);
	pthread_mutex_unlock(&sim->counter_mutex);
	}

}

bool is_in_top(t_heap *heap, t_coder *coder)
{
    if (heap->size == 0)
        return (false);
    if (heap->arr[0].coder == coder)
        return (true);
    return (false);
}

bool heap_peek(t_coder *coder, t_dongle *dongle)
{
    if (dongle->waiting_heap.size == 0)
        return (false);
    else if (is_in_top(&dongle->waiting_heap, coder) && !dongle->in_use && get_timestamp_ms() >= dongle->available_at)
    {
        return (true);
    }
    return (false);
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
	while (coder->compiles_done < sim->number_of_compiles_required)
	{
		if (!acquire_dongle(coder, first))
			break ;
		print_coder_state(coder, "acquired first dongle");
		if (second != first)
		{
			if (!acquire_dongle(coder, second))
			{
				release_dongle(coder, first);
				break ;
			}
		}
		print_coder_state(coder, "acquired second dongle");
		coder->last_compile_start = get_timestamp_ms();
		print_coder_state(coder, "compiling");
		sleep_ms(sim->time_to_compile);
		release_dongle(coder, first);
		if (second != first)
			release_dongle(coder, second);
		coder->compiles_done++;
		print_coder_state(coder, "debugging");
		sleep_ms(sim->time_to_debug);
		print_coder_state(coder, "refactoring");
		sleep_ms(sim->time_to_refactor);
	}
	return (NULL);
}
