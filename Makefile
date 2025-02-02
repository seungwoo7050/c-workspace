NAME := libftprintf.a

CC := cc
CFLAGS := -Wall -Wextra -Werror
CPPFLAGS := -Iinclude
AR := ar
ARFLAGS := rcs
RM := rm -f

SRC := src/ft_printf.c \
	src/ft_output.c \
	src/ft_parse.c \
	src/ft_measure.c \
	src/ft_dispatch.c \
	src/ft_text.c \
	src/ft_numeric_layout.c \
	src/ft_number.c \
	src/ft_hex.c
OBJ := $(SRC:.c=.o)
HEADER := include/ft_printf.h src/ft_printf_internal.h
TEST_BIN := tests/bin/test_ft_printf
FAULT_TEST_BIN := tests/bin/test_output_faults

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

test: $(NAME)
	mkdir -p tests/bin
	$(CC) $(CFLAGS) $(CPPFLAGS) tests/test_ft_printf.c $(NAME) -o $(TEST_BIN)
	./$(TEST_BIN)
	$(CC) $(CFLAGS) $(CPPFLAGS) -DFT_PRINTF_TEST_WRITE \
		tests/test_output_faults.c $(SRC) -o $(FAULT_TEST_BIN)
	./$(FAULT_TEST_BIN)

clean:
	$(RM) $(OBJ)
	rm -rf tests/bin

fclean: clean
	$(RM) $(NAME) $(TEST_BIN) $(FAULT_TEST_BIN)

re: fclean all

.PHONY: all clean fclean re test
