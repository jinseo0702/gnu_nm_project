/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/27 15:14:05 by jinseo            #+#    #+#             */
/*   Updated: 2024/02/29 17:54:43 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	unsigned char	*cpy;
	size_t			cnt;

	cpy = (unsigned char *)s;
	cnt = 0;
	while (cnt < n)
	{
		*(cpy + cnt) = 0;
		cnt++;
	}
}
