CC := cc
CFLAGS := -std=c99 -Wall -Wextra -Werror -Wpedantic
CPPFLAGS := -Iinclude
OBJ_DIR := .build

MODEL_SRCS := src/stack.c
MODEL_OBJS := $(MODEL_SRCS:src/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean fclean re

all: $(MODEL_OBJS)

$(OBJ_DIR)/%.o: src/%.c include/push_swap.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f push_swap checker

re: fclean all
