/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libftprintf.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 22:52:07 by jinseo            #+#    #+#             */
/*   Updated: 2024/04/07 15:14:21 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFTPRINTF_H
# define LIBFTPRINTF_H
# include <unistd.h>
# include <stdarg.h>

void	*ft_memset(void *s, int c, size_t n);
int		ft_printf(const char *str, ...);
int		ft_putstr(const char *str);
int		ft_putchar(unsigned char c);
int		ft_putaddress(void *ptr);
int   ft_putaddress_upper(void *ptr);
int		ft_putnbr(int n);
int		ft_putnbr_un(unsigned int n);
int		ft_put_hex(unsigned int n);
int		ft_put_hex_upper(unsigned int n);
int		ft_put_octal(unsigned int n);
int		ft_put_space(unsigned int n);

int   ft_fprintf(size_t fd, const char *str, ...);
int   ft_fputstr(const char *s, size_t fd);
int   ft_fputchar(unsigned char c, size_t fd);
int	  ft_fputaddress(void *ptr, size_t fd);
int   ft_fputaddress_upper(void *ptr, size_t fd);
int	  ft_fputnbr(int n, size_t fd);
int	  ft_fputnbr_un(unsigned int n, size_t fd);
int	  ft_fput_hex(unsigned int n, size_t fd);
int	  ft_fput_hex_upper(unsigned int n, size_t fd);
int	  ft_fput_octal(unsigned int n, size_t fd);
int	  ft_fput_space(unsigned int n, size_t fd);

#endif
