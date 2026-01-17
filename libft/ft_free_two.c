/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_two.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 18:47:16 by jinseo            #+#    #+#             */
/*   Updated: 2024/12/01 10:00:31 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_free_two(char **arry)
{
	int	idx;

	idx = 0;
	while (arry[idx])
	{
		ft_freenull(&arry[idx]);
		arry[idx] = NULL;
		idx++;
	}
	free(arry);
}
