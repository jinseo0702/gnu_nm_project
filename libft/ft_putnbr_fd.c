/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 14:28:33 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/04 16:45:15 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	long int	nb;
	long int	div;
	char		x;

	nb = (long int)n;
	div = 1;
	if (n < 0)
	{
		ft_putchar_fd('-', fd);
		nb *= -1;
	}
	while (nb / div > 9)
		div *= 10;
	while (div > 0)
	{
		x = (nb / div) + 48;
		ft_putchar_fd(x, fd);
		nb %= div;
		div /= 10;
	}
}
