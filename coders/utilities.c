/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilities.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 17:30:11 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/01 17:30:11 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

static int	count_args(char **args)
{
	int	len;

	len = 0;
	while (args[len])
		len++;
	return (len);
}

bool	str_checker(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!(str[i] >= '0' && str[i] <= '9') && str[i] != ' ' && str[i] != '-'
			&& str[i] != '+')
		{
			fprintf(stderr, "Error occured while checking,"
				"arguments passed are invalid!!\n"
				"Please give valid arguments\n");
			fprintf(stderr,
				"found a non numeric character in one of arguments\n"
				"\nquitting...\n");
			return (false);
		}
		i++;
	}
	return (true);
}

static bool	check_scheduler(char *scheduler)
{
	if ((strcmp(scheduler, "fifo") && (strcmp(scheduler, "edf"))))
	{
		fprintf(stderr, "error occured when checking for scheduler type\n");
		fprintf(stderr, "Please provide a valid Scheduler (fifo or edf)\n");
		fprintf(stderr,
			"Example:\n./codexion 5 800 200 200 150 7 50 fifo (or edf)\n"
			"\nquitting...\n");
		return (false);
	}
	return (true);
}

bool	arg_checker(char **args)
{
	int	i;
	int	len;

	i = 1;
	len = count_args(args);
	while (i < len - 1)
	{
		if (!str_checker(args[i]))
			return (false);
		i++;
	}
	if (len != 9)
	{
		fprintf(stderr,
			"Error occured while checking,"
			"number of arguments passed is invalid!!\n"
			"Please give valid arguments\n");
		fprintf(stderr,
			"Example:\n\n./codexion 5 800 200 200 150 7 50 fifo (or edf)\n"
			"\nquitting...\n");
		return (false);
	}
	return (check_scheduler(args[8]));
}
