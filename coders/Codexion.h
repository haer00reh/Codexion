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

bool str_checker(char *str);
bool arg_checker(char **args);
long	ft_atol(char *str);

typedef struct s_dongle
{
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	bool				in_use;
	long				available_at;
	int					id;
}	t_dongle;

typedef struct s_request
{
	int		coder_id;
	long	priority;
}	t_request;

typedef struct s_heap
{
	t_request	*arr;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_simulation
{
	struct s_coder	*coders;
	t_dongle		*dongles;
	pthread_t		monitor_thread;
	long			simulation_start_time;
	bool			stop;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	counter_mutex;
	long			fifo_sequence;

	long	number_of_coders;
	long	number_of_compiles_required;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	long	dongle_cooldown;
	char	*schedule;
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

bool	arg_to_long(char *str, long *ret);
bool	init_simulation_from_args(t_simulation *sim, char **av);
void	destroy_simulation_runtime(t_simulation *sim);
bool init_coder(t_simulation *sim);
bool init_dongles(t_simulation *sim);

#endif
