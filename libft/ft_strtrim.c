/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/01 23:22:18 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/08 05:01:19 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_check(char c, char *set)
{
	while (*set)
	{
		if (c == *set)
			return (1);
		set++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*ptr2;
	int		len1;
	int		len2;

	len1 = 0;
	len2 = ft_strlen(s1) - 1;
	while (ft_check(*(char *)(s1 + len1), (char *)set))
		len1++;
	while (ft_check(*(char *)(s1 + len2), (char *)set))
		len2--;
	if (len2 < len1)
	{
		ptr2 = (char *)ft_calloc(1, sizeof(char));
		if (!ptr2)
			return (NULL);
	}
	else
	{
		ptr2 = (char *)malloc(sizeof(char) * len2 - len1 + 2);
		ft_strlcpy(ptr2, s1 + len1, len2 - len1 + 2);
		if (!ptr2)
			return (NULL);
	}
	return (ptr2);
}
