/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup_flag.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 18:13:57 by jinseo            #+#    #+#             */
/*   Updated: 2025/03/27 20:17:50 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup_flag(const char *s, int *status)
{
	char	*dest;
	int		i;

	dest = (char *)malloc((sizeof(char) * ft_strlen(s)) + 1);
	i = 0;
	if (dest == NULL)
	{
		*status = -1;
		exit(1);
	}
	while (*(s + i) != '\0')
	{
		*(dest + i) = *(s + i);
		i++;
	}
	*(dest + i) = '\0';
	return (dest);
}
