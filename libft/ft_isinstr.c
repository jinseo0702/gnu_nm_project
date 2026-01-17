/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isinstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 14:25:11 by jinseo            #+#    #+#             */
/*   Updated: 2025/03/27 20:19:23 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isinstr(int c, const char *str)
{
	unsigned char	as;

	as = (unsigned char)c;
	while (*str)
	{
		if (as == *str)
			return (1);
		str++;
	}
	return (0);
}
