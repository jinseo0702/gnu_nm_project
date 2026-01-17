/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr_len.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 21:23:31 by jinseo            #+#    #+#             */
/*   Updated: 2024/12/01 10:03:22 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strchr_len(const char *s, int c)
{
	char	*scpy;
	size_t	idx;

	scpy = (char *)s;
	idx = 0;
	while (*scpy)
	{
		if (*scpy == (unsigned char)c)
			return (idx);
		idx++;
		scpy++;
	}
	if (c == 0)
		return (0);
	return (0);
}
