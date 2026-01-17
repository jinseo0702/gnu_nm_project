/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jinseo <jinseo@student.42gyeongsan.kr      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 00:44:03 by jinseo            #+#    #+#             */
/*   Updated: 2024/03/09 17:03:12 by jinseo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list *dst;

	dst = *lst;
	if (*lst == NULL && new == NULL)
	{
		(*lst) -> next = new;
		*lst = dst;
		return ;
	}
	if (lst == NULL || new == NULL)
		return ;
	while ((*lst) -> next)
		*lst = (*lst) -> next;
	(*lst) -> next = new;
	*lst = dst;
}
