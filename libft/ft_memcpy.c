/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 15:25:49 by jinseo            #+#    #+#             */
/*   Updated: 2024/02/27 20:31:02 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char	*cpy;
	size_t			cnt;

	cpy = (unsigned char *)dest;
	cnt = 0;
	while (cnt < n)
	{
		*(cpy + cnt) = *(unsigned char *)(src + cnt);
		cnt++;
	}
	return (dest);
}
