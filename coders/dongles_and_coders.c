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
		return ;
	}
	pthread_mutex_unlock(&coder->sim->stop_mutex);
	timestamp = get_timestamp_ms();
	printf("%ld %d %s\n", timestamp - coder->sim->simulation_start_time,
		coder->id, state);
	pthread_mutex_unlock(&coder->sim->print_mutex);
}

void	request_submission(t_simulation *sim, t_coder *coder, t_dongle *dongle)
{
	long	priority;

	pthread_mutex_lock(&sim->counter_mutex);
	if (sim->scheduler == FIFO)
		priority = 0;
	else
		priority = coder->last_compile_start + sim->time_to_burnout;
	heap_push(dongle->waiting_heap, coder, priority, sim->global_sequence++);
	pthread_mutex_unlock(&sim->counter_mutex);
}
