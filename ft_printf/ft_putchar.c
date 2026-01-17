/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 13:58:58 by jinseo            #+#    #+#             */
/*   Updated: 2024/04/07 14:21:27 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

int	ft_putchar(unsigned char c)
{
	ssize_t	cnt;

	cnt = 0;
	cnt = write(1, &c, 1);
	return ((int)cnt);
}

int	ft_fputchar(unsigned char c, size_t fd)
{
	ssize_t	cnt;

	cnt = 0;
	cnt = write(fd, &c, 1);
	return ((int)cnt);
}

