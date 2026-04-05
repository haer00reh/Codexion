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

bool request_less(t_request a, t_request b)
{
	if (a.priority < b.priority)
		return (true);
	if (a.priority == b.priority && a.sequence < b.sequence)
		return (true);
	return (false);
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
		if (left < heap->size
			&& request_less(heap->arr[left], heap->arr[smallest]))
			smallest = left;
		if (right < heap->size
			&& request_less(heap->arr[right], heap->arr[smallest]))
			smallest = right;
		if (smallest == child)
			break ;
		swap_requests(&heap->arr[child], &heap->arr[smallest]);
		child = smallest;
	}
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

bool	heap_pop_min(t_heap *heap, t_request *out)
{
	if (heap->size == 0)
		return (false);
	*out = heap->arr[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->arr[0] = heap->arr[heap->size];
		heapify_down(heap, 0);
	}
	return (true);
}

// debugging


// int	main(void)
// {
// 	t_heap		heap;
// 	t_request	out;

// 	t_coder c1 = {.id = 1};
// 	t_coder c2 = {.id = 2};
// 	t_coder c3 = {.id = 3};
// 	t_coder c4 = {.id = 4};

// 	printf("==== TEST 1: priority order ====\n");

// 	heap_init(&heap, 10);

// 	heap_push(&heap, &c1, 30, 1);
// 	heap_push(&heap, &c2, 10, 2);
// 	heap_push(&heap, &c3, 20, 3);

// 	while (heap_pop_min(&heap, &out))
// 		printf("id=%d prio=%ld seq=%ld\n",
// 			out.coder->id,
// 			out.priority,
// 			out.sequence);

// 	heap_destroy(&heap);

// 	printf("\n==== TEST 2: same priority, sequence order ====\n");

// 	heap_init(&heap, 10);

// 	heap_push(&heap, &c1, 10, 2);
// 	heap_push(&heap, &c2, 10, 1);
// 	heap_push(&heap, &c3, 10, 3);
// 	heap_push(&heap, &c4, 10, 0);

// 	while (heap_pop_min(&heap, &out))
// 		printf("id=%d prio=%ld seq=%ld\n",
// 			out.coder->id,
// 			out.priority,
// 			out.sequence);

// 	heap_destroy(&heap);

// 	return (0);
// }
