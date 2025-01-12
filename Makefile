NAME := libft.a

CC := cc
CFLAGS := -Wall -Wextra -Werror -std=c99 -pedantic
CPPFLAGS := -I.
DEPFLAGS := -MMD -MP
AR := ar
ARFLAGS := rcs
RM := rm -f
RMDIR := rm -rf
MKDIR := mkdir -p

SRC := \
	src/char/ft_char.c \
	src/memory/ft_memory_fill.c \
	src/memory/ft_memory_copy.c \
	src/memory/ft_memory_move.c \
	src/memory/ft_memory_scan.c \
	src/string/ft_string_bounds.c \
	src/string/ft_string_search.c \
	src/string/ft_string_build.c \
	src/string/ft_split.c \
	src/string/ft_string_transform.c \
	src/convert/ft_atoi.c \
	src/convert/ft_itoa.c \
	src/alloc/ft_allocate.c \
	src/io/ft_fd_output.c
OBJ_DIR := build/obj
OBJ := $(SRC:%.c=$(OBJ_DIR)/%.o)
DEP := $(OBJ:.o=.d)
TEST_BIN := tests/bin/test_libft
TEST_SRC := $(wildcard tests/test_*.c)

.PHONY: all clean fclean re test

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(OBJ_DIR)/%.o: %.c libft.h
	@$(MKDIR) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(TEST_BIN): $(NAME) $(TEST_SRC) tests/test.h
	@$(MKDIR) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(TEST_SRC) $(NAME) -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	$(RMDIR) build tests/bin

fclean: clean
	$(RM) $(NAME)

re: fclean all

-include $(DEP)
