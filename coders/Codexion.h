/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 17:30:13 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/01 17:30:13 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>

typedef struct s_coder t_coder;
typedef struct s_heap t_heap;
typedef struct s_request t_request;

bool str_checker(char *str);
bool arg_checker(char **args);
long	ft_atol(char *str);
long	get_timestamp_ms(void);

typedef struct s_request
{
	t_coder *coder;
	long sequence;
	long	priority;
}	t_request;

enum scheduler_type
{
	FIFO,
	EDF
};

typedef struct s_heap
{
	t_request	*arr;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	t_heap				*waiting_heap;
	bool				in_use;
	long				available_at;
	int					id;
}	t_dongle;

typedef struct s_simulation
{
	t_coder			*coders;
	t_dongle			*dongles;
	pthread_t			monitor_thread;
	long				simulation_start_time;
	bool				stop;
	pthread_mutex_t		print_mutex;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	counter_mutex;
	pthread_mutex_t read_write_mutex;
	long			global_sequence;

	long	number_of_coders;
	long	number_of_compiles_required;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	long	dongle_cooldown;
	enum scheduler_type	scheduler;
} 	t_simulation;

typedef struct s_coder
{
	int					id;
	pthread_t			thread;
	long				last_compile_start;
	int					compiles_done;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	struct s_simulation	*sim;
}	t_coder;

bool request_less(t_request a, t_request b);
bool	heap_init(t_heap *heap, int capacity);
void	heap_destroy(t_heap *heap);
bool	heap_push(t_heap *heap, t_coder *coder, long priority, long sequence);
bool	heap_pop_min(t_heap *heap);
void swap_requests(t_request *a, t_request *b);
bool arg_to_long(char *str, long *ret);
void	heapify_down(t_heap *heap, int child);
void	heapify_up(t_heap *heap, int child);
bool	arg_to_long(char *str, long *ret);
bool	init_simulation_from_args(t_simulation *sim, char **av);
void	destroy_simulation_runtime(t_simulation *sim);
bool init_coder(t_simulation *sim);
bool init_dongles(t_simulation *sim);
void request_submission(t_simulation *sim, t_coder *coder, t_dongle *dongle);
bool can_take_dongle(t_coder *coder, t_dongle *dongle);
bool can_take_dongle(t_coder *coder, t_dongle *dongle);
bool acquire_dongle(t_coder *coder, t_dongle *dongle);
void release_dongle(t_coder *coder, t_dongle *dongle);
void *runtime_coder_routine(void *arg);
void *burn_out_monitor(void *arg);
void	print_coder_state(t_coder *coder, const char *state);

#endif
