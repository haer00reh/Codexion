/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_entities.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:00:00 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/26 10:00:00 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

static bool	dongle_heap_init_failed(t_simulation *sim, int i)
{
	free(sim->dongles[i].waiting_heap);
	sim->dongles[i].waiting_heap = NULL;
	while (i > 0)
	{
		heap_destroy(sim->dongles[i - 1].waiting_heap);
		free(sim->dongles[i - 1].waiting_heap);
		sim->dongles[i - 1].waiting_heap = NULL;
		pthread_mutex_destroy(&sim->dongles[i - 1].mutex);
		pthread_cond_destroy(&sim->dongles[i - 1].cond);
		i--;
	}
	free(sim->dongles);
	free(sim->coders);
	return (false);
}

bool	init_dongles(t_simulation *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->number_of_coders);
	if (!sim->dongles)
		return (free(sim->coders), false);
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->dongles[i].waiting_heap = malloc(sizeof(t_heap));
		if (!sim->dongles[i].waiting_heap
			|| !heap_init(sim->dongles[i].waiting_heap, sim->number_of_coders))
			return (dongle_heap_init_failed(sim, i));
		sim->dongles[i].id = i + 1;
		sim->dongles[i].in_use = false;
		sim->dongles[i].available_at = 0;
		pthread_mutex_init(&sim->dongles[i].mutex, NULL);
		pthread_cond_init(&sim->dongles[i].cond, NULL);
		i++;
	}
	return (true);
}

bool	init_coder(t_simulation *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->number_of_coders);
	if (!sim->coders)
		return (false);
	i = 0;
	while (i < sim->number_of_coders)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].thread = 0;
		sim->coders[i].last_compile_start = 0;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1)
			% sim->number_of_coders];
		sim->coders[i].sim = sim;
		i++;
	}
	return (true);
}
