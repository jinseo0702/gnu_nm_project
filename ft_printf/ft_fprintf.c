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

static int	ft_ftype(va_list lst_va, const char *str, size_t fd)
{
	int	cnt;

	cnt = 0;
	if (*(str + 1) == 'c')
		cnt += ft_fputchar((unsigned char)va_arg(lst_va, int), fd);
	else if (*(str + 1) == 's')
		cnt += ft_fputstr(va_arg(lst_va, char *), fd);
	else if (*(str + 1) == 'p')
		cnt += ft_fputaddress(va_arg(lst_va, void *), fd);
	else if (*(str + 1) == 'P')
		cnt += ft_fputaddress_upper(va_arg(lst_va, void *), fd);
	else if (*(str + 1) == 'd' || *(str + 1) == 'i')
		cnt += ft_fputnbr(va_arg(lst_va, int), fd);
	else if (*(str + 1) == 'u')
		cnt += ft_fputnbr_un(va_arg(lst_va, unsigned int), fd);
	else if (*(str + 1) == 'x')
		cnt += ft_fput_hex(va_arg(lst_va, unsigned int), fd);
	else if (*(str + 1) == 'X')
		cnt += ft_fput_hex_upper(va_arg(lst_va, unsigned int), fd);
	else if (*(str + 1) == 'o')
		cnt += ft_fput_octal(va_arg(lst_va, unsigned int), fd);
	else if (*(str + 1) == '*')
		cnt += ft_fput_space(va_arg(lst_va, unsigned int),fd);
	else if (*(str + 1) == '%')
		cnt += ft_fputchar('%', fd);
	else
		cnt = -1;
	return (cnt);
}

static int	ft_fcallstr(va_list lst_va, const char *str, size_t fd)
{
	int	cnt2;
	int	flag;

	cnt2 = 0;
	flag = 0;
	while (*str != '\0' && cnt2 != -1)
	{
		if (*str == '%')
		{
			flag = ft_ftype(lst_va, str ,fd);
			str++;
		}
		else
			flag = ft_fputchar(*str, fd);
		cnt2 += flag;
		str++;
		if (flag == -1)
			return (-1);
	}
	return (cnt2);
}

int	ft_fprintf(size_t fd, const char *str, ...)
{
	va_list	lst_va;
	int		cnt;
	ssize_t	fd_close;

	cnt = 0;
	fd_close = 0;
	fd_close = write(fd, "wtf value", 0);
	if (fd_close == -1 || str == NULL)
		return (-1);
	va_start(lst_va, str);
	cnt = 0;
	cnt = ft_fcallstr(lst_va, str, fd);
	va_end(lst_va);
	return (cnt);
}

