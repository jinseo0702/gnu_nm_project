/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/26 19:43:47 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/03 11:32:13 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_isalpha(int c)
{
	unsigned char	as;

	as = (unsigned char)c;
	if ((as >= 'a' && as <= 'z') || (as >= 'A' && as <= 'Z'))
		return (1024);
	return (0);
}
