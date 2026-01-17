/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 15:31:56 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/05 17:07:34 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t	cnt1;

	cnt1 = 0;
	while (cnt1 < n)
	{
		if (*(unsigned char *)(s + cnt1) == (unsigned char)c)
			return ((void *)(s + cnt1));
		cnt1++;
	}
	return (NULL);
}
