/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_tools.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 17:30:11 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/26 10:00:00 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"

bool	arg_to_long(char *str, long *ret)
{
	*ret = ft_atol(str);
	if (*ret < 0)
	{
		fprintf(stderr,
			"Error occured while checking, found a negative number\n"
			"Please provide a positive number\n\nquitting...\n");
		return (false);
	}
	return (true);
}

long	get_timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long	ft_atol(char *str)
{
	long	sign;
	long	nbr;

	sign = 1;
	nbr = 0;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	if (*str < '0' || *str > '9')
		return (0);
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (0);
		nbr = (nbr * 10) + (*str - '0');
		if ((sign == 1 && nbr > 2147483647) || (sign == -1 && nbr > 2147483648))
			return (0);
		str++;
	}
	return (nbr * sign);
}
