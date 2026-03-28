#include "Codexion.h"

static void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heapify_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (heap->arr[parent].priority <= heap->arr[index].priority)
			break ;
		swap_requests(&heap->arr[parent], &heap->arr[index]);
		index = parent;
	}
}

void	heapify_down(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		left = index * 2 + 1;
		right = index * 2 + 2;
		smallest = index;
		if (left < heap->size
			&& heap->arr[left].priority < heap->arr[smallest].priority)
			smallest = left;
		if (right < heap->size
			&& heap->arr[right].priority < heap->arr[smallest].priority)
			smallest = right;
		if (smallest == index)
			break ;
		swap_requests(&heap->arr[index], &heap->arr[smallest]);
		index = smallest;
	}
}

int	heap_init(t_heap *heap, int capacity)
{
	heap->arr = malloc(sizeof(t_request) * capacity);
	if (!heap->arr)
		return (0);
	heap->size = 0;
	heap->capacity = capacity;
	return (1);
}

void	heap_destroy(t_heap *heap)
{
	free(heap->arr);
	heap->arr = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

int	heap_push(t_heap *heap, int coder_id, long priority)
{
	if (heap->size >= heap->capacity)
		return (0);
	heap->arr[heap->size].coder_id = coder_id;
	heap->arr[heap->size].priority = priority;
	heapify_up(heap, heap->size);
	heap->size++;
	return (1);
}

int	heap_pop_min(t_heap *heap, t_request *out)
{
	if (heap->size == 0)
		return (0);
	*out = heap->arr[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->arr[0] = heap->arr[heap->size];
		heapify_down(heap, 0);
	}
	return (1);
}


//debug func
void	print_heap_array(t_heap *heap)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		printf("[%d: coder=%d prio=%ld] ",
			i,
			heap->arr[i].coder_id,
			heap->arr[i].priority);
		i++;
	}
	printf("\n");
}
