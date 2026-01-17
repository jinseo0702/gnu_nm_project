/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 16:51:32 by jinseo            #+#    #+#             */
/*   Updated: 2025/03/27 20:18:29 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	cnt;

	cnt = 0;
	if (size <= 0)
		return (ft_strlen(src));
	while (*(src + cnt) && cnt + 1 < size)
	{
		*(dst + cnt) = *(src + cnt);
		cnt++;
	}
	*(dst + cnt) = '\0';
	return (ft_strlen(src));
}
