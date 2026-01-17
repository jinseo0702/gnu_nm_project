/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 15:33:56 by jinseo            #+#    #+#             */
/*   Updated: 2024/02/28 17:09:27 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t	cnt;
	size_t	ncpy;
	size_t	nncpy;

	cnt = 0;
	ncpy = n;
	nncpy = n;
	while (cnt < n)
	{
		if (dest == (src + cnt))
		{
			while (ncpy-- > 0)
				*(unsigned char *)(dest + ncpy) = \
				*(unsigned char *)(src + --nncpy);
			return (dest);
		}
		cnt++;
	}
	cnt = 0;
	while (cnt < n)
	{
		*(unsigned char *)(dest + cnt) = *(unsigned char *)(src + cnt);
		cnt++;
	}
	return (dest);
}
