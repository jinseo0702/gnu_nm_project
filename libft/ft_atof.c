/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 14:55:42 by jinseo            #+#    #+#             */
/*   Updated: 2024/07/31 15:51:27 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	is_minus(char **nptr)
{
	int	mi;

	mi = 1;
	while ((**nptr >= 9 && **nptr <= 13) || **nptr == 32)
		(*nptr)++;
	while (**nptr == '+' || **nptr == '-')
	{
		if (**nptr == '-')
			mi *= -1;
		(*nptr)++;
	}
	return (mi);
}

double	ft_atof(char *nptr)
{
	double	nb;
	double	fraction;
	int		mi;

	nb = 0.0;
	fraction = 0.1;
	mi = is_minus(&nptr);
	while (ft_isdigit(*nptr))
	{
		nb = (*nptr - 48) + (nb * 10);
		nptr++;
	}
	if (*nptr == '.')
	{
		nptr++;
		while (ft_isdigit(*nptr))
		{
			nb += (*nptr - 48) * fraction;
			fraction *= 0.1;
			nptr++;
		}
	}
	return (nb * mi);
}
