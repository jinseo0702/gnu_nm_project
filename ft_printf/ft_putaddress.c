/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putaddress.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 13:58:56 by jinseo            #+#    #+#             */
/*   Updated: 2024/04/07 14:20:55 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

int	ft_putaddress(void *ptr)
{
	int					cnt_put_address;
	ssize_t				re_cnt;
	unsigned long int	address;
	char				hex[16];

	ft_memset(hex, 0, 16);
	address = 0;
	re_cnt = 0;
	address = (unsigned long int)ptr;
	cnt_put_address = 16;
	if (address == 0)
	{
		re_cnt = write(1, "(nil)", 5);
		return ((int)re_cnt);
	}
	while (address > 0)
	{
		hex[--cnt_put_address] = "0123456789abcdef"[address % 16];
		address /= 16;
	}
	re_cnt += write(1, "0x", 2);
	re_cnt += write(1, &hex[cnt_put_address], 16 - cnt_put_address);
	return ((int)re_cnt);
}

int	ft_putaddress_upper(void *ptr)
{
	int					cnt_put_address;
	ssize_t				re_cnt;
	unsigned long int	address;
	char				hex[16];

	ft_memset(hex, 0, 16);
	address = 0;
	re_cnt = 0;
	address = (unsigned long int)ptr;
	cnt_put_address = 16;
	if (address == 0)
	{
		re_cnt = write(1, "(nil)", 5);
		return ((int)re_cnt);
	}
	while (address > 0)
	{
		hex[--cnt_put_address] = "0123456789ABCDEF"[address % 16];
		address /= 16;
	}
	re_cnt += write(1, "0x", 2);
	re_cnt += write(1, &hex[cnt_put_address], 16 - cnt_put_address);
	return ((int)re_cnt);
}

int	ft_put_hex(unsigned int n)
{
	int		cnt_put_hex;
	ssize_t	re_cnt_hex;
	char	hex[8];

	ft_memset(hex, 0, 8);
	re_cnt_hex = 0;
	cnt_put_hex = 8;
	if (n == 0)
	{
		re_cnt_hex += write(1, "0", 1);
		return (re_cnt_hex);
	}
	while (n > 0)
	{
		hex[--cnt_put_hex] = "0123456789abcdef"[n % 16];
		n /= 16;
	}
	re_cnt_hex += write(1, &hex[cnt_put_hex], 8 - cnt_put_hex);
	return ((int)re_cnt_hex);
}

int	ft_put_hex_upper(unsigned int n)
{
	int		cnt_put_hex_up;
	ssize_t	re_cnt_hex_up;
	char	hex_up[8];

	ft_memset(hex_up, 0, 8);
	re_cnt_hex_up = 0;
	cnt_put_hex_up = 8;
	if (n == 0)
	{
		re_cnt_hex_up += write(1, "0", 1);
		return (re_cnt_hex_up);
	}
	while (n > 0)
	{
		hex_up[--cnt_put_hex_up] = "0123456789ABCDEF"[n % 16];
		n /= 16;
	}
	re_cnt_hex_up += write(1, &hex_up[cnt_put_hex_up], 8 - cnt_put_hex_up);
	return ((int)re_cnt_hex_up);
}

int	ft_put_octal(unsigned int n)
{
	int		cnt_put_octal;
	ssize_t	re_cnt_octal;
	char	octal[8];

	ft_memset(octal, 0, 8);
	re_cnt_octal = 0;
	cnt_put_octal = 8;
	if (n == 0)
	{
		re_cnt_octal += write(1, "0", 1);
		return (re_cnt_octal);
	}
	while (n > 0)
	{
		octal[--cnt_put_octal] = "01234567"[n % 8];
		n /= 8;
	}
	re_cnt_octal += write(1, &octal[cnt_put_octal], 8 - cnt_put_octal);
	return ((int)re_cnt_octal);
}

int	ft_fputaddress(void *ptr, size_t fd)
{
	int					cnt_put_address;
	ssize_t				re_cnt;
	unsigned long int	address;
	char				hex[16];

	ft_memset(hex, 0, 16);
	address = 0;
	re_cnt = 0;
	address = (unsigned long int)ptr;
	cnt_put_address = 16;
	if (address == 0)
	{
		re_cnt = write(fd, "(nil)", 5);
		return ((int)re_cnt);
	}
	while (address > 0)
	{
		hex[--cnt_put_address] = "0123456789abcdef"[address % 16];
		address /= 16;
	}
	re_cnt += write(fd, "0x", 2);
	re_cnt += write(fd, &hex[cnt_put_address], 16 - cnt_put_address);
	return ((int)re_cnt);
}

int	ft_fputaddress_upper(void *ptr, size_t fd)
{
	int					cnt_put_address;
	ssize_t				re_cnt;
	unsigned long int	address;
	char				hex[16];

	ft_memset(hex, 0, 16);
	address = 0;
	re_cnt = 0;
	address = (unsigned long int)ptr;
	cnt_put_address = 16;
	if (address == 0)
	{
		re_cnt = write(fd, "(nil)", 5);
		return ((int)re_cnt);
	}
	while (address > 0)
	{
		hex[--cnt_put_address] = "0123456789ABCDEF"[address % 16];
		address /= 16;
	}
	re_cnt += write(fd, "0x", 2);
	re_cnt += write(fd, &hex[cnt_put_address], 16 - cnt_put_address);
	return ((int)re_cnt);
}

int	ft_fput_hex(unsigned int n, size_t fd)
{
	int		cnt_put_hex;
	ssize_t	re_cnt_hex;
	char	hex[8];

	ft_memset(hex, 0, 8);
	re_cnt_hex = 0;
	cnt_put_hex = 8;
	if (n == 0)
	{
		re_cnt_hex += write(fd, "0", 1);
		return (re_cnt_hex);
	}
	while (n > 0)
	{
		hex[--cnt_put_hex] = "0123456789abcdef"[n % 16];
		n /= 16;
	}
	re_cnt_hex += write(fd, &hex[cnt_put_hex], 8 - cnt_put_hex);
	return ((int)re_cnt_hex);
}

int	ft_fput_hex_upper(unsigned int n, size_t fd)
{
	int		cnt_put_hex_up;
	ssize_t	re_cnt_hex_up;
	char	hex_up[8];

	ft_memset(hex_up, 0, 8);
	re_cnt_hex_up = 0;
	cnt_put_hex_up = 8;
	if (n == 0)
	{
		re_cnt_hex_up += write(fd, "0", 1);
		return (re_cnt_hex_up);
	}
	while (n > 0)
	{
		hex_up[--cnt_put_hex_up] = "0123456789ABCDEF"[n % 16];
		n /= 16;
	}
	re_cnt_hex_up += write(fd, &hex_up[cnt_put_hex_up], 8 - cnt_put_hex_up);
	return ((int)re_cnt_hex_up);
}

int	ft_fput_octal(unsigned int n, size_t fd)
{
	int		cnt_put_octal;
	ssize_t	re_cnt_octal;
	char	octal[8];

	ft_memset(octal, 0, 8);
	re_cnt_octal = 0;
	cnt_put_octal = 8;
	if (n == 0)
	{
		re_cnt_octal += write(fd, "0", 1);
		return (re_cnt_octal);
	}
	while (n > 0)
	{
		octal[--cnt_put_octal] = "01234567"[n % 8];
		n /= 8;
	}
	re_cnt_octal += write(fd, &octal[cnt_put_octal], 8 - cnt_put_octal);
	return ((int)re_cnt_octal);
}