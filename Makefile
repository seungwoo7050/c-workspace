BIN_DIR := build/bin
NAME := $(BIN_DIR)/push_swap
CHECKER := $(BIN_DIR)/checker
CC := cc
CFLAGS := -std=c99 -Wall -Wextra -Werror -Wpedantic
CPPFLAGS := -Iinclude
OBJ_DIR := build/obj
FAULT_DIR := build/fault
TEST_DIR := build/test

SRC := $(wildcard src/*.c)
COMMON_SRCS := $(filter-out src/push_swap.c src/sort.c src/checker.c src/checker_reader.c,$(SRC))
PUSH_SRCS := src/push_swap.c src/sort.c
CHECKER_SRCS := src/checker.c src/checker_reader.c
COMMON_OBJS := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(COMMON_SRCS))
PUSH_OBJS := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(PUSH_SRCS))
CHECKER_OBJS := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(CHECKER_SRCS))
OPERATION_TEST := $(TEST_DIR)/operation_invariants
FAULT_COMMON_OBJS := $(patsubst src/%.c,$(FAULT_DIR)/%.o,$(COMMON_SRCS))
FAULT_PUSH_OBJS := $(patsubst src/%.c,$(FAULT_DIR)/%.o,$(PUSH_SRCS))
FAULT_CHECKER_OBJS := $(patsubst src/%.c,$(FAULT_DIR)/%.o,$(CHECKER_SRCS))
FAULT_PUSH_SWAP := $(FAULT_DIR)/push_swap
FAULT_CHECKER := $(FAULT_DIR)/checker

.DELETE_ON_ERROR:
.PHONY: all clean fclean re test

all: $(NAME) $(CHECKER)

$(NAME): $(COMMON_OBJS) $(PUSH_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(CHECKER): $(COMMON_OBJS) $(CHECKER_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: src/%.c include/push_swap.h | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

$(OPERATION_TEST): tests/operation_invariants.c $(OBJ_DIR)/stack.o \
		$(OBJ_DIR)/operations.o $(OBJ_DIR)/runtime.o $(OBJ_DIR)/utils.o | $(TEST_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(FAULT_PUSH_SWAP): $(FAULT_COMMON_OBJS) $(FAULT_PUSH_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(FAULT_CHECKER): $(FAULT_COMMON_OBJS) $(FAULT_CHECKER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(FAULT_DIR)/%.o: src/%.c include/push_swap.h | $(FAULT_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -DPS_FAULT_INJECTION -MMD -MP -c $< -o $@

$(OBJ_DIR) $(FAULT_DIR) $(TEST_DIR) $(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf build tests/__pycache__ .pytest_cache

fclean: clean

re: fclean all

test: all $(OPERATION_TEST) $(FAULT_PUSH_SWAP) $(FAULT_CHECKER)
	$(OPERATION_TEST)
	python3 tests/run_tests.py
	PS_PUSH_SWAP=$(FAULT_PUSH_SWAP) PS_CHECKER=$(FAULT_CHECKER) \
		python3 tests/fault_tests.py

-include $(COMMON_OBJS:.o=.d) $(PUSH_OBJS:.o=.d) \
	$(CHECKER_OBJS:.o=.d) $(FAULT_COMMON_OBJS:.o=.d) \
	$(FAULT_PUSH_OBJS:.o=.d) $(FAULT_CHECKER_OBJS:.o=.d)
