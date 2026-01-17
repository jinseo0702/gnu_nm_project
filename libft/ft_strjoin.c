/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/01 18:10:59 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/01 18:11:03 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*reptr;
	size_t	len;

	len = ft_strlen(s1) + ft_strlen(s2);
	reptr = (char *)malloc((sizeof(char) * len) + 1);
	if (!reptr)
		return (NULL);
	ft_strlcpy(reptr, s1, ft_strlen(s1) + 1);
	ft_strlcat((reptr + ft_strlen(s1)), s2, ft_strlen(s2) + 1);
	return (reptr);
}
