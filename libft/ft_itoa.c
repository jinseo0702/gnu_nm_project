/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 19:14:28 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/08 04:59:37 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_in_nbr(char *arr, int n, int *index)
{
	if (n < 0)
	{
		arr[*index] = '-';
		(*index)++;
		n *= -1;
	}
	if (n >= 10)
	{
		ft_in_nbr(arr, n / 10, index);
		(*index)++;
		n %= 10;
		n += 48;
		arr[*index] = n;
	}
	else
	{
		n += 48;
		arr[*index] = n;
	}
}

char	*ft_itoa(int n)
{
	char	arr[12];
	char	*str;
	int		index;

	ft_memset(arr, 0, 12);
	index = 0;
	if (n == -2147483648)
	{
		str = (char *)malloc(sizeof(char) * 12);
		if (!str)
			return (NULL);
		ft_strlcpy(str, "-2147483648", 12);
		return (str);
	}
	ft_in_nbr(arr, n, &index);
	str = (char *)malloc(sizeof(char) * (ft_strlen(arr) + 1));
	if (!str)
		return (NULL);
	ft_strlcpy(str, arr, (ft_strlen(arr) + 1));
	return (str);
}
