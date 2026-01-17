/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 17:50:58 by jinseo            #+#    #+#             */
/*   Updated: 2025/03/27 20:16:50 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	**ft_strfree(char **str, int stri)
{
	int	i;
	int	size;

	i = 0;
	size = 0;
	while (size <= stri)
	{
		free(str[i]);
		i++;
		size++;
	}
	free(str);
	return (NULL);
}

static int	ft_wordcount(char const *s, char c)
{
	int	cnt;
	int	i;

	cnt = 0;
	i = 0;
	while (s[i] != '\0')
	{
		if ((i == 0 && s[i] != c) || (i != 0 && s[i] != c && s[i - 1] == c))
			cnt++;
		i++;
	}
	return (cnt);
}

static int	ft_len(const char *s, char c)
{
	int	i;

	i = 0;
	while (s[i] != c && s[i] != '\0')
		i++;
	return (i);
}

static char	*ft_save(const char *s, char c)
{
	char	*str;
	int		len;
	int		i;

	len = ft_len(s, c);
	i = 0;
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		exit(1);
	while (i < len)
	{
		str[i] = s[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}

char	**ft_split(char const *s, char c)
{
	char	**str;
	int		cnt;
	int		i;
	int		stri;

	cnt = ft_wordcount(s, c);
	i = 0;
	stri = 0;
	str = (char **)malloc(sizeof(char *) * (cnt + 1));
	if (!str)
		exit(1);
	while (s[i])
	{
		if ((i == 0 && s[i] != c) || (s[i] != c && s[i - 1] == c && i != 0))
		{
			str[stri] = ft_save(&s[i], c);
			if (str[stri] == NULL)
				return (ft_strfree(str, stri));
			stri++;
		}
		i++;
	}
	str[stri] = NULL;
	return (str);
}
