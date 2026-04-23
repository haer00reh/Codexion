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

bool	str_checker(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (!(str[i] >= '0' && str[i] <= '9') && str[i] != ' ' && str[i] != '-'
			&& str[i] != '+')
		{
			fprintf(stderr,
					"Error occured while checking,"
					"arguments passed are invalid!!\nPlease give valid arguments\n");
			fprintf(stderr,
				"found a non numeric character in one of arguments\n\nquitting...\n");
			return (false);
		}
		i++;
	}
	return (true);
}

bool	arg_checker(char **args)
{
	int(i), (len);
	i = 1;
	len = 0;
	while (args[len])
		len++;
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
				"number of arguments passed is invalid!!\nPlease give valid arguments\n");
		fprintf(stderr,
			"Example:\n\n./codexion 5 800 200 200 150 7 50 fifo (or edf)\n\nquitting...\n");
		return (false);
	}
	else if ((strcmp(args[8], "fifo") && (strcmp(args[8], "edf"))))
	{
		fprintf(stderr, "error occured when checking for scheduler type\n");
		fprintf(stderr, "Please provide a valid Scheduler (fifo or edf)\n");
		fprintf(stderr,
			"Example:\n./codexion 5 800 200 200 150 7 50 fifo (or edf)\n\nquitting...\n");
		return (false);
	}
	else
		return (true);
}
