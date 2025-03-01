NAME := push_swap
CHECKER := checker
CC := cc
CFLAGS := -std=c99 -Wall -Wextra -Werror -Wpedantic
CPPFLAGS := -Iinclude
RM := rm -f
RMDIR := rm -rf
OBJ_DIR := .build

COMMON_SRCS := \
	src/parser.c \
	src/stack.c \
	src/operations.c \
	src/runtime.c \
	src/utils.c
COMMON_OBJS := $(COMMON_SRCS:src/%.c=$(OBJ_DIR)/%.o)
PUSH_SRCS := src/push_swap.c src/sort.c
PUSH_OBJS := $(PUSH_SRCS:src/%.c=$(OBJ_DIR)/%.o)
CHECKER_SRCS := src/checker.c src/checker_reader.c
CHECKER_OBJS := $(CHECKER_SRCS:src/%.c=$(OBJ_DIR)/%.o)
OPERATION_TEST := $(OBJ_DIR)/operation_invariants

.DELETE_ON_ERROR:
.PHONY: all clean fclean re test

all: $(COMMON_OBJS) $(PUSH_OBJS) $(CHECKER_OBJS) $(NAME) $(CHECKER)

$(NAME): $(COMMON_OBJS) $(PUSH_OBJS)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(PUSH_OBJS) -o $@

$(CHECKER): $(COMMON_OBJS) $(CHECKER_OBJS)
	$(CC) $(CFLAGS) $(COMMON_OBJS) $(CHECKER_OBJS) -o $@

$(OBJ_DIR)/%.o: src/%.c include/push_swap.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OPERATION_TEST): tests/operation_invariants.c \
		$(OBJ_DIR)/stack.o $(OBJ_DIR)/operations.o $(OBJ_DIR)/runtime.o \
		$(OBJ_DIR)/utils.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RMDIR) $(OBJ_DIR) tests/__pycache__ .pytest_cache

fclean: clean
	$(RM) $(NAME) $(CHECKER)

re: fclean all

test: all $(OPERATION_TEST)
	$(OPERATION_TEST)
	python3 tests/run_tests.py
