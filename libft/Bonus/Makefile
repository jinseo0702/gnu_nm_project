CC = cc
CFLAG = -Wall -Wextra -Werror
AR = ar -rcs
RM = rm -rf

SRCS = ft_lstnew.c \
	   ft_lstadd_front.c \
	ft_lstsize.c \
	ft_lstlast.c \
	ft_lstadd_back.c \
	ft_lstdelone.c \
##	ft_lstclear.c \
	ft_lstiter.c \
	ft_lstmap.c

OBJS = $(SRCS:.c=.o)
NAME = libft.a
HEADER = libft.h


all : $(NAME)

$(NAME) : $(OBJS)
	$(AR) $@ $^

%.o : %.c $(HEADER)
	$(CC) $(CFLAG) -c $< -o $@

clean :
	$(RM) $(OBJS)

fclean : 
	$(RM) $(OBJS) $(NAME)

re : 
	make fclean 
	make all

.PHONY: all clean fclean re
