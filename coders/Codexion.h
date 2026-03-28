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
	int					in_use;
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

#endif
