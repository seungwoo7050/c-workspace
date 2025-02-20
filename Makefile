CC := cc
CFLAGS := -std=c99 -Wall -Wextra -Werror -Wpedantic
CPPFLAGS := -Iinclude
OBJ_DIR := .build

COMMON_SRCS := \
	src/stack.c \
	src/operations.c \
	src/utils.c
COMMON_OBJS := $(COMMON_SRCS:src/%.c=$(OBJ_DIR)/%.o)
OPERATION_TEST := $(OBJ_DIR)/operation_invariants

.PHONY: all clean fclean re test

all: $(COMMON_OBJS)

$(OBJ_DIR)/%.o: src/%.c include/push_swap.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OPERATION_TEST): tests/operation_invariants.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f push_swap checker

re: fclean all

test: all $(OPERATION_TEST)
	$(OPERATION_TEST)
