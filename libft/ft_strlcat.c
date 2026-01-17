/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 17:31:19 by jinseo            #+#    #+#             */
/*   Updated: 2024/02/29 21:58:48 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	dstlen;
	size_t	srclen;
	size_t	templen;

	dstlen = ft_strlen(dst);
	templen = dstlen;
	srclen = 0;
	if (dstlen >= size)
		return (size + ft_strlen(src));
	while (*(src + srclen) && (templen + srclen + 1 < size))
	{
		*(dst + dstlen) = *(src + srclen);
		dstlen++;
		srclen++;
	}
	*(dst + dstlen) = '\0';
	return (templen + ft_strlen(src));
}
