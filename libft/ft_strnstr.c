/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 23:36:08 by jinseo            #+#    #+#             */
/*   Updated: 2024/02/29 14:09:54 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	cnt1;
	size_t	cnt2;

	cnt1 = 0;
	if (!*(little))
		return ((char *)big);
	while (*(big + cnt1) && cnt1 < len)
	{
		cnt2 = 0;
		while (*(big + cnt1) == *(little + cnt2) && cnt1 < len)
		{
			if ((cnt2 + 1) == ft_strlen(little))
				return ((char *)(big + cnt1 - ft_strlen(little) + 1));
			cnt1++;
			cnt2++;
		}
		cnt1 -= cnt2;
		cnt1++;
	}
	return (NULL);
}
