CC = gcc
CFLAG = -g -Wall -Wextra -Werror
CFLAG += $(EXTRA_CFLAG)
AR = ar -rcs
RM = rm -rf

SRCS = ft_isalnum.c \
	ft_isalpha.c \
	ft_isascii.c \
	ft_isinstr.c \
	ft_atoi.c \
	ft_atoi_longlong.c \
	ft_atof.c \
	ft_bzero.c \
	ft_isprint.c \
	ft_isdigit.c \
	ft_memchr.c \
	ft_memcmp.c \
	ft_memcpy.c \
	ft_memmove.c \
	ft_memset.c \
	ft_onlyisspace.c \
	ft_strchr.c \
	ft_strlcpy.c \
	ft_strlcat.c \
	ft_strlen.c \
	ft_strncmp.c \
	ft_strnstr.c \
	ft_strrchr.c \
	ft_tolower.c \
	ft_toupper.c \
	ft_calloc.c \
	ft_strdup.c \
	ft_strdup_flag.c \
	ft_substr.c \
	ft_strjoin.c \
	ft_strtrim.c \
	ft_split.c \
	ft_split_str.c \
	ft_itoa.c \
	ft_putchar_fd.c \
	ft_putstr_fd.c \
	ft_putendl_fd.c \
	ft_putnbr_fd.c \
	ft_strmapi.c \
	ft_striteri.c \
	ft_isspace.c \
	ft_strndup.c \
	ft_freenull.c \
	ft_free_two.c \
	ft_strchr_len.c \
	ft_strrstr.c \
	gnl_check_bonus/get_next_line.c \
	gnl_check_bonus/get_next_line_utils.c \
	gnl_check_bonus/get_next_line_bonus.c \
	gnl_check_bonus/get_next_line_utils_bonus.c

OBJS = $(SRCS:.c=.o)
NAME = libft.a
HEADER = libft.h


all : $(NAME)

$(NAME) : $(OBJS)
	@$(AR) $@ $^

%.o : %.c $(HEADER)
	@$(CC) $(CFLAG) -c $< -o $@

clean :
	@$(RM) $(OBJS)

fclean : 
	@$(RM) $(OBJS) $(NAME)

re : 
	@make fclean 
	@make all

.PHONY: all clean fclean re
