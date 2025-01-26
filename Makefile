NAME := libftprintf.a

CC := cc
CFLAGS := -Wall -Wextra -Werror
CPPFLAGS := -Iinclude
AR := ar
ARFLAGS := rcs
RM := rm -f

SRC := src/ft_printf.c \
	src/ft_output.c
OBJ := $(SRC:.c=.o)
HEADER := include/ft_printf.h src/ft_printf_internal.h

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
	rm -rf tests/bin

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
