/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 22:04:31 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/03 11:35:06 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*scpy;
	int		len;
	size_t	cnt;

	scpy = (char *)s;
	len = ft_strlen(s);
	cnt = 0;
	while (len >= 0 && cnt <= ft_strlen(scpy))
	{
		if (*(scpy + len) == (unsigned char)c)
			return (scpy + len);
		len--;
		cnt++;
	}
	return (NULL);
}
