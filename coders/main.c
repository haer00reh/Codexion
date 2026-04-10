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
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
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

	for (int i = 0; i < sim.number_of_coders; i++)
	{
	printf("coder %d: left dongle %d, right dongle %d\n",
			sim.coders[i].id,
			sim.coders[i].left_dongle->id,
			sim.coders[i].right_dongle->id);
	}
	destroy_simulation_runtime(&sim);
	return (0);
}
