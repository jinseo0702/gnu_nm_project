#include "libft.h"
#include <stdio.h>
int	main()
{
	int	len = 0;
	int	idx = 0;
	t_list	*lst;
	t_list	*last;

	lst = ft_lstnew("abcde");
	ft_lstadd_front(&lst, ft_lstnew("Alphabet 5word is "));
	//ft_lstadd_back
	ft_lstadd_back(&lst, ft_lstnew("is good word end."));
	last = lst;
	len = ft_lstsize(lst);
	//ft_lstlast == NULL is right!
	printf("%s \n", ft_lstlast(last));
	//ft_lstsize
	printf("lst size is%d\n", len);
	//linked list's detail
	while(lst)
	{
		printf("%s\n", (char *)lst->content);
		lst = lst->next;
	}
	return (0);
}
