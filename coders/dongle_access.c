/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_access.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:00:00 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/26 10:00:00 by haer-reh         ###   ########.fr       */
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
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
}

bool	can_take_dongle(t_coder *coder, t_dongle *dongle)
{
	if (dongle->waiting_heap->size == 0)
		return (false);
	if (dongle->in_use)
		return (false);
	if (get_timestamp_ms() < dongle->available_at)
		return (false);
	if (dongle->waiting_heap->arr[0].coder != coder)
		return (false);
	return (true);
}

bool	acquire_dongle(t_coder *coder, t_dongle *dongle)
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
		if (can_take_dongle(coder, dongle) && dongle->in_use == false)
		{
			dongle->in_use = true;
			heap_pop_min(dongle->waiting_heap);
			pthread_mutex_unlock(&dongle->mutex);
			return (true);
		}
		wait_dongle_until_ready(dongle);
	}
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->in_use = false;
	dongle->available_at = get_timestamp_ms() + coder->sim->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
