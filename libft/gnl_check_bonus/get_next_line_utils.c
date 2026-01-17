/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 15:07:19 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/25 11:28:33 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*free_backup(char **backup)
{
	if (*backup != NULL)
		free(*backup);
	*backup = NULL;
	return (NULL);
}

char	*ft_gnl_strjoin(char **backup, const char *buf)
{
	char	*reptr;
	size_t	len;
	size_t	ind;

	ind = 0;
	len = ft_gnl_strlen(*backup) + ft_gnl_strlen(buf);
	reptr = (char *)malloc(sizeof(char) * (len + 1));
	if (!reptr)
		return (NULL);
	while ((*backup)[ind] != '\0')
	{
		reptr[ind] = (*backup)[ind];
		ind++;
	}
	while (*buf != '\0')
	{
		reptr[ind] = *buf;
		ind++;
		buf++;
	}
	reptr[ind] = '\0';
	if (*backup != NULL)
		free_backup(backup);
	return (reptr);
}

ssize_t	ft_gnl_strlen(const char *str)
{
	ssize_t	len;

	len = 0;
	while (str[len] != '\0')
		len++;
	return (len);
}

char	*ft_gnl_strdup(char **backup, ssize_t len)
{
	char	*dest;
	ssize_t	i;

	i = 0;
	dest = (char *)malloc((sizeof(char) * (ft_gnl_strlen(*backup) + 2)));
	if (!dest)
		return (NULL);
	while ((*backup)[i] != '\0' && i < len)
	{
		*(dest + i) = (*backup)[i];
		i++;
	}
	*(dest + i) = '\0';
	return (dest);
}

char	*ft_gnl_substr(char **backup, unsigned int start)
{
	char	*str;
	size_t	len;
	size_t	init_len;

	if (ft_gnl_strlen(*backup) < start)
	{
		str = (char *)malloc(sizeof(char) * 2);
		free_backup(backup);
		str[0] = '\0';
		return (str);
	}
	len = ft_gnl_strlen((*backup) + start);
	init_len = 0;
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	while (init_len < len && (*backup)[start])
	{
		str[init_len] = (*backup)[start];
		init_len++;
		start++;
	}
	str[init_len] = '\0';
	free_backup(backup);
	return (str);
}
