/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 13:59:06 by jinseo            #+#    #+#             */
/*   Updated: 2024/04/07 15:25:45 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

int	ft_putnbr(int n)
{
	long int	nb;
	long int	div;
	int			cnt_nbr;
	char		x;

	nb = (long int)n;
	div = 1;
	cnt_nbr = 0;
	if (n < 0)
	{
		cnt_nbr += ft_putchar('-');
		nb *= -1;
	}
	while (nb / div > 9)
		div *= 10;
	while (div > 0)
	{
		x = (nb / div) + 48;
		cnt_nbr += ft_putchar((unsigned char)x);
		nb %= div;
		div /= 10;
	}
	return (cnt_nbr);
}

int	ft_putnbr_un(unsigned int n)
{
	unsigned int	nb_un;
	unsigned int	div_un;
	int				cnt_nbr_un;
	char			x_un;

	nb_un = n;
	div_un = 1;
	cnt_nbr_un = 0;
	while (nb_un / div_un > 9)
		div_un *= 10;
	while (div_un > 0)
	{
		x_un = (nb_un / div_un) + 48;
		cnt_nbr_un += ft_putchar((unsigned char)x_un);
		nb_un %= div_un;
		div_un /= 10;
	}
	return (cnt_nbr_un);
}

int	ft_fputnbr(int n, size_t fd)
{
	long int	nb;
	long int	div;
	int			cnt_nbr;
	char		x;

	nb = (long int)n;
	div = 1;
	cnt_nbr = 0;
	if (n < 0)
	{
		cnt_nbr += ft_putchar('-');
		nb *= -1;
	}
	while (nb / div > 9)
		div *= 10;
	while (div > 0)
	{
		x = (nb / div) + 48;
		cnt_nbr += ft_fputchar((unsigned char)x, fd);
		nb %= div;
		div /= 10;
	}
	return (cnt_nbr);
}

int	ft_fputnbr_un(unsigned int n, size_t fd)
{
	unsigned int	nb_un;
	unsigned int	div_un;
	int				cnt_nbr_un;
	char			x_un;

	nb_un = n;
	div_un = 1;
	cnt_nbr_un = 0;
	while (nb_un / div_un > 9)
		div_un *= 10;
	while (div_un > 0)
	{
		x_un = (nb_un / div_un) + 48;
		cnt_nbr_un += ft_fputchar((unsigned char)x_un, fd);
		nb_un %= div_un;
		div_un /= 10;
	}
	return (cnt_nbr_un);
}