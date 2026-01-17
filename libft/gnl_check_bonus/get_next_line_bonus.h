/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 18:23:48 by jinseo            #+#    #+#             */
/*   Updated: 2024/04/05 20:33:54 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

char	*get_next_line(int fd);
char	*ft_get_head(char **backup, ssize_t re_frf);
ssize_t	ft_new_line(const char *str, ssize_t read_len);
ssize_t	ft_read_file(int fd, char **backup);
ssize_t	ft_read_sub(int fd, char *buf);
char	*ft_gnl_substr(char **backup, unsigned int start);
char	*ft_gnl_strdup(char **backup, ssize_t len);
ssize_t	ft_gnl_strlen(const char *str);
char	*ft_gnl_strjoin(char **backup, const char *buf);
char	*free_backup(char **backup);

#endif
