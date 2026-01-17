/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 21:23:31 by jinseo            #+#    #+#             */
/*   Updated: 2024/02/29 21:42:48 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	char	*scpy;

	scpy = (char *)s;
	while (*scpy)
	{
		if (*scpy == (unsigned char)c)
			return (scpy);
		scpy++;
	}
	if (c == 0)
		return (scpy);
	return (NULL);
}
