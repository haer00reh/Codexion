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

# include <stdbool.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_coder		t_coder;
typedef struct s_heap		t_heap;
typedef struct s_request	t_request;

typedef enum e_scheduler_type
{
	FIFO,
	EDF
}	t_scheduler_type;

typedef struct s_request
{
	t_coder	*coder;
	long	sequence;
	long	priority;
}	t_request;

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
	t_coder				*coders;
	t_dongle			*dongles;
	pthread_t			monitor_thread;
	long				simulation_start_time;
	bool				stop;
	pthread_mutex_t		print_mutex;
	pthread_mutex_t		stop_mutex;
	pthread_mutex_t		counter_mutex;
	pthread_mutex_t		read_write_mutex;
	long				global_sequence;
	long				number_of_coders;
	long				number_of_compiles_required;
	long				time_to_burnout;
	long				time_to_compile;
	long				time_to_debug;
	long				time_to_refactor;
	long				dongle_cooldown;
	t_scheduler_type	scheduler;
}	t_simulation;

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

bool	request_less(t_request a, t_request b);
bool	heap_init(t_heap *heap, int capacity);
void	heap_destroy(t_heap *heap);
bool	heap_push(t_heap *heap, t_coder *coder, long priority, long sequence);
bool	heap_pop_min(t_heap *heap);

void	swap_requests(t_request *a, t_request *b);
void	heapify_up(t_heap *heap, int child);
void	heapify_down(t_heap *heap, int child);

bool	str_checker(char *str);
bool	arg_checker(char **args);
bool	arg_to_long(char *str, long *ret);
long	ft_atol(char *str);
long	get_timestamp_ms(void);

bool	init_simulation_from_args(t_simulation *sim, char **av);
void	destroy_simulation_runtime(t_simulation *sim);
bool	init_dongles(t_simulation *sim);
bool	init_coder(t_simulation *sim);
bool	run_coder_cycle(t_coder *coder, t_dongle *first, t_dongle *second);

void	request_submission(t_simulation *sim, t_coder *coder, t_dongle *dongle);
bool	can_take_dongle(t_coder *coder, t_dongle *dongle);
bool	acquire_dongle(t_coder *coder, t_dongle *dongle);
void	release_dongle(t_coder *coder, t_dongle *dongle);

void	*runtime_coder_routine(void *arg);
void	*burn_out_monitor(void *arg);
void	print_coder_state(t_coder *coder, const char *state);

void	wait_dongle_until_ready(t_dongle *dongle);
void	set_ordered_dongles(t_coder *coder, t_dongle **first,
			t_dongle **second);
void	sleep_ms(long ms, t_simulation *sim);
bool	take_dongles(t_coder *coder, t_dongle *first, t_dongle *second);
bool	handle_single_coder(t_coder *coder, t_dongle *first);
bool	should_stop_coder(t_coder *coder);
bool	start_coder_threads(t_simulation *sim);
void	join_coder_threads(t_simulation *sim);
void	init_compile_starts(t_simulation *sim);
int		run_simulation(t_simulation *sim);
bool	coder_burned_out(t_simulation *sim, int i, long *current, int *id);
int		check_simulation_state(t_simulation *sim, long *current,
			int *coder_id);
void	finish_simulation(t_simulation *sim, int state, long current,
			int coder_id);
bool	dongle_heap_init_failed(t_simulation *sim, int i);
bool	init_mutexes(t_simulation *sim);
bool	parse_numeric_values(t_simulation *sim, char **av);
bool	set_scheduler(t_simulation *sim, char *scheduler);
bool	simulation_stopped(t_simulation *sim);
bool	free_everything(t_simulation *sim);

#endif
