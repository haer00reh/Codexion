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

bool	request_less(t_request a, t_request b)
{
	if (a.priority < b.priority)
		return (true);
	if (a.priority == b.priority && a.sequence < b.sequence)
		return (true);
	return (false);
}

bool	heap_init(t_heap *heap, int capacity)
{
	heap->arr = malloc(sizeof(t_request) * capacity);
	if (!heap->arr)
		return (false);
	heap->size = 0;
	heap->capacity = capacity;
	return (true);
}

void	heap_destroy(t_heap *heap)
{
	free(heap->arr);
	heap->arr = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

bool	heap_push(t_heap *heap, t_coder *coder, long priority, long sequence)
{
	if (heap->size >= heap->capacity)
		return (false);
	heap->arr[heap->size].sequence = sequence;
	heap->arr[heap->size].coder = coder;
	heap->arr[heap->size].priority = priority;
	heapify_up(heap, heap->size);
	heap->size++;
	return (true);
}

bool	heap_pop_min(t_heap *heap)
{
	if (heap->size == 0)
		return (false);
	heap->size--;
	heap->arr[0] = heap->arr[heap->size];
	heapify_down(heap, 0);
	return (true);
}
