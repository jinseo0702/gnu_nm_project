/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 17:15:04 by jinseo            #+#    #+#             */
/*   Updated: 2025/03/27 20:15:39 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
  void	*reptr;
	size_t	total;

  if (nmemb == 0 || size == 0)
    return (malloc(0));
	total = nmemb * size;
	if (total != 0 && total / nmemb != size)
		return (NULL);
	reptr = (void *)malloc(size * nmemb);
	if (reptr == NULL) {
    return (NULL);
  };
	ft_memset(reptr, 0, size * nmemb);
	return (reptr);
}
