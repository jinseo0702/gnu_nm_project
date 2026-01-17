/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/01 15:05:52 by jinseo            #+#    #+#             */
/*   Updated: 2025/03/27 20:18:45 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*dst;

	if (!*s)
	{
		dst = (char *)ft_calloc(start + len + 1, sizeof(char));
		if (!dst)
			exit(1);
		return (dst);
	}
	dst = (char *)ft_calloc(start + len + 1, sizeof(char));
	ft_strlcpy(dst, s + start, len + 1);
	if (len == 0)
		return (dst);
	if (start > ft_strlen(s))
	{
		ft_bzero(dst, 1);
		return (dst);
	}
	if (!*dst)
		return (NULL);
	return (dst);
}
