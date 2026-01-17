/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/07 13:58:50 by jinseo            #+#    #+#             */
/*   Updated: 2024/04/07 16:42:35 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libftprintf.h"

static int	ft_type(va_list lst_va, const char *str)
{
	int	cnt;

	cnt = 0;
	if (*(str + 1) == 'c')
		cnt += ft_putchar((unsigned char)va_arg(lst_va, int));
	else if (*(str + 1) == 's')
		cnt += ft_putstr(va_arg(lst_va, char *));
	else if (*(str + 1) == 'p')
		cnt += ft_putaddress(va_arg(lst_va, void *));
	else if (*(str + 1) == 'P')
		cnt += ft_putaddress_upper(va_arg(lst_va, void *));
	else if (*(str + 1) == 'd' || *(str + 1) == 'i')
		cnt += ft_putnbr(va_arg(lst_va, int));
	else if (*(str + 1) == 'u')
		cnt += ft_putnbr_un(va_arg(lst_va, unsigned int));
	else if (*(str + 1) == 'x')
		cnt += ft_put_hex(va_arg(lst_va, unsigned int));
	else if (*(str + 1) == 'X')
		cnt += ft_put_hex_upper(va_arg(lst_va, unsigned int));
	else if (*(str + 1) == 'o')
		cnt += ft_put_octal(va_arg(lst_va, unsigned int));
	else if (*(str + 1) == '*')
		cnt += ft_put_space(va_arg(lst_va, unsigned int));
	else if (*(str + 1) == '%')
		cnt += ft_putchar('%');
	else
		cnt = -1;
	return (cnt);
}

static int	ft_callstr(va_list lst_va, const char *str)
{
	int	cnt2;
	int	flag;

	cnt2 = 0;
	flag = 0;
	while (*str != '\0' && cnt2 != -1)
	{
		if (*str == '%')
		{
			flag = ft_type(lst_va, str);
			str++;
		}
		else
			flag = ft_putchar(*str);
		cnt2 += flag;
		str++;
		if (flag == -1)
			return (-1);
	}
	return (cnt2);
}

int	ft_printf(const char *str, ...)
{
	va_list	lst_va;
	int		cnt;
	ssize_t	fd_close;

	cnt = 0;
	fd_close = 0;
	fd_close = write(1, "wtf value", 0);
	if (fd_close == -1 || str == NULL)
		return (-1);
	va_start(lst_va, str);
	cnt = 0;
	cnt = ft_callstr(lst_va, str);
	va_end(lst_va);
	return (cnt);
}

