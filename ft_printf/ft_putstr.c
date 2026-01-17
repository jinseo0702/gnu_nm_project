/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 13:59:09 by jinseo            #+#    #+#             */
/*   Updated: 2024/04/07 14:28:56 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

int	ft_putstr(const char *s)
{
	ssize_t	cnt;

	cnt = 0;
	if (s == NULL)
	{
		cnt += write(1, "(null)", 6);
		return ((int)cnt);
	}
	while (*s)
	{
		cnt += write(1, s, 1);
		s++;
	}
	return ((int)cnt);
}

int	ft_fputstr(const char *s, size_t fd)
{
	ssize_t	cnt;

	cnt = 0;
	if (s == NULL)
	{
		cnt += write(fd, "(null)", 6);
		return ((int)cnt);
	}
	while (*s)
	{
		cnt += write(fd, s, 1);
		s++;
	}
	return ((int)cnt);
}

int ft_put_space(unsigned int n) {
	ssize_t	cnt;

	cnt = 0;
	if (n == 0) {
		return ((int)cnt);
	}
	for (unsigned int i = 0; i < n; i++) {
		cnt += write(1, " ", 1);
	}
	return cnt;
}

int ft_fput_space(unsigned int n, size_t fd) {
	ssize_t	cnt;

	cnt = 0;
	if (n == 0) {
		return ((int)cnt);
	}
	for (unsigned int i = 0; i < n; i++) {
		cnt += write(fd, " ", 1);
	}
	return cnt;
}
