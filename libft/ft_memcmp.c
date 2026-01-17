/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 16:48:56 by jinseo            #+#    #+#             */
/*   Updated: 2024/02/29 20:49:46 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char	*s11;
	unsigned char	*s22;
	size_t			cnt;

	s11 = (unsigned char *)s1;
	s22 = (unsigned char *)s2;
	cnt = 0;
	while (cnt < n)
	{
		if (*s11 != *s22)
			return (*s11 - *s22);
		s22++;
		s11++;
		cnt++;
	}
	return (0);
}
