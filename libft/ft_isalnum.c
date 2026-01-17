/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalnum.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 20:26:10 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/03 11:31:50 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalnum(int c)
{
	unsigned char	as;

	as = (unsigned char)c;
	if ((as >= 'a' && as <= 'z') || (as >= 'A' && as <= 'Z'))
		return (8);
	if (as >= '0' && as <= '9')
		return (10);
	return (0);
}
