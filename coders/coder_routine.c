/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:00:00 by haer-reh          #+#    #+#             */
/*   Updated: 2026/05/01 16:22:44 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

void	set_ordered_dongles(t_coder *coder, t_dongle **first,
		t_dongle **second)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
	else
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
}

bool	handle_single_coder(t_coder *coder, t_dongle *first)
{
	if (!acquire_dongle(coder, first))
		return (false);
	if (!simulation_stopped(coder->sim))
		print_coder_state(coder, "has taken a dongle");
	return (true);
}

bool	should_stop_coder(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->counter_mutex);
	if (coder->compiles_done >= coder->sim->number_of_compiles_required)
	{
		pthread_mutex_unlock(&coder->sim->counter_mutex);
		return (true);
	}
	pthread_mutex_unlock(&coder->sim->counter_mutex);
	if (simulation_stopped(coder->sim))
		return (true);
	return (false);
}

void	*runtime_coder_routine(void *arg)
{
	t_coder		*coder;
	t_dongle	*first;
	t_dongle	*second;

	coder = (t_coder *)arg;
	set_ordered_dongles(coder, &first, &second);
	pthread_mutex_lock(&coder->sim->counter_mutex);
	coder->compiles_done = 0;
	coder->has_finished = false;
	pthread_mutex_unlock(&coder->sim->counter_mutex);
	if (coder->sim->number_of_coders == 1)
	{
		handle_single_coder(coder, first);
		return (NULL);
	}
	while (!should_stop_coder(coder))
	{
		if (!run_coder_cycle(coder, first, second))
			break ;
		if (coder->compiles_done == coder->sim->number_of_compiles_required)
			coder->has_finished = true;
	}
	return (NULL);
}
