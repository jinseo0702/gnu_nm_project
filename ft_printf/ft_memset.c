/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 20:41:46 by jinseo            #+#    #+#             */
/*   Updated: 2024/04/07 14:06:04 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

void	*ft_memset(void *s, int c, size_t n)
{
	size_t			cnt;
	unsigned char	*cpy;
	unsigned char	as;

	cpy = (unsigned char *)s;
	as = (unsigned char)c;
	cnt = 0;
	while (cnt < n)
	{
		*(cpy + cnt) = as;
		cnt++;
	}
	return (cpy);
}
