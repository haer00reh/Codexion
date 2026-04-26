/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 10:00:00 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/26 10:00:00 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heapify_up(t_heap *heap, int child)
{
	int	parent;

	while (child > 0)
	{
		parent = (child - 1) / 2;
		if (!request_less(heap->arr[child], heap->arr[parent]))
			break ;
		swap_requests(&heap->arr[parent], &heap->arr[child]);
		child = parent;
	}
}

void	heapify_down(t_heap *heap, int child)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		left = child * 2 + 1;
		right = child * 2 + 2;
		smallest = child;
		if (left < heap->size && request_less(heap->arr[left],
				heap->arr[smallest]))
			smallest = left;
		if (right < heap->size && request_less(heap->arr[right],
				heap->arr[smallest]))
			smallest = right;
		if (smallest == child)
			break ;
		swap_requests(&heap->arr[child], &heap->arr[smallest]);
		child = smallest;
	}
}
