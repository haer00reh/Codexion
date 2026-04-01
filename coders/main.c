/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haer-reh <haer-reh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 17:30:03 by haer-reh          #+#    #+#             */
/*   Updated: 2026/04/01 17:30:03 by haer-reh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Codexion.h"


int main(int ac, char **av)
{
	t_simulation	sim;

	(void)ac;
	memset(&sim, 0, sizeof(sim));

	if (!(arg_checker(av)))
		return -1;
	if (!init_simulation_from_args(&sim, av))
		return -1;

	for (int i = 0; i < sim.number_of_coders; i++)
	{
	printf("coder %d: left dongle %d, right dongle %d\n",
			sim.coders[i].id,
			sim.coders[i].left_dongle->id,
			sim.coders[i].right_dongle->id);
	}
	destroy_simulation_runtime(&sim);
	return (0);
}
