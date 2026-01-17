/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 23:26:03 by jinseo            #+#    #+#             */
/*   Updated: 2024/02/28 23:31:21 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned char	*s11;
	unsigned char	*s22;
	size_t			cnt;

	if (!s1 || !s2)
		return (0);
	s11 = (unsigned char *)s1;
	s22 = (unsigned char *)s2;
	cnt = 0;
	while ((*s11 || *s22) && cnt < n)
	{
		if (*s11 != *s22)
			return (*s11 - *s22);
		s22++;
		s11++;
		cnt++;
	}
	return (0);
}
