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

void request_submission(t_simulation *sim, t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
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
	pthread_mutex_unlock(&dongle->mutex);

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
    else if (is_in_top(&dongle->waiting_heap, coder) && !dongle->in_use && get_current_time() >= dongle->available_at)
    {
        return (true);
    }
    return (false);
}
