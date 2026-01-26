CC = gcc
RM = rm -rf

NAME = nm

DEBUG ?= 0

CFLAG = -Wall -Wextra -Werror
CFLAG += -Wno-unused-parameter
CFLAG += -Iinclude -Ilibft -Ift_printf
ifeq ($(DEBUG),1)
CFLAG += -g -DDEBUG
endif

LIBFT_A = libft/libft.a
PRINTF_A = ft_printf/libftprintf.a

SRC = src/nm.c \
	src/arg.c \
	src/io_unit.c \
	src/format_router.c \
	src/ar_parser.c \
	src/elf_parser.c \
	src/sym_classify.c \
	src/sort_filter_print.c

OBJS = $(SRC:.c=.o)

HEADER = include/nm.h \
	libft/libft.h \
	ft_printf/libftprintf.h

all : $(NAME)

$(NAME): $(OBJS) $(LIBFT_A) $(PRINTF_A)
	@$(CC) -o $@ $^

debug: fclean
	@make DEBUG=1 all

$(LIBFT_A):
	@make -C libft/

$(PRINTF_A):
	@make -C ft_printf/

%.o : %.c $(HEADER)
	@$(CC) $(CFLAG) -c $< -o $@

clean :
	@make clean -C libft/
	@make clean -C ft_printf/
	@$(RM) $(OBJS)

fclean :
	@make fclean -C libft/
	@make fclean -C ft_printf/
	@$(RM) $(OBJS) $(NAME)

re :
	@make fclean
	@make all

.PHONY: all debug clean fclean re