CC      	?= cc
CFLAGS  	?= -std=c99 -Wall -Wextra -Werror -pedantic -Iinclude -MMD -MP
AR      	?= ar
ARFLAGS 	?= rcs
ASAN_FLAGS  ?= -fsanitize=address,undefined -g -O0

SRCS    	:= $(wildcard src/*/*.c)
BIN_DIR 	:= build
TARGET  	:= libft.a
OBJS    	:= $(patsubst %.c,$(BIN_DIR)/%.o,$(SRCS))
DEPS    	:= $(OBJS:.o=.d)

TEST_SRC		:= $(wildcard tests/test_*.c)
TEST_BIN		:= $(BIN_DIR)/test_libft
TEST_ASAN_BIN	:= $(BIN_DIR)/test_libft_asan

.PHONY: all test test-asan clean re

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%.o: %.c include/libft.h | $(BIN_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


$(BIN_DIR)/$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

test: $(TEST_BIN)
	@$(TEST_BIN)
	@printf 'libft tests: PASS\n'

test-asan: $(TEST_ASAN_BIN)
	@$(TEST_ASAN_BIN)
	@printf 'libft tests (ASAN): PASS\n'

$(TEST_BIN): $(BIN_DIR)/$(TARGET) $(TEST_SRC) include/libft.h tests/test.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $(TEST_SRC) $(BIN_DIR)/$(TARGET) -o $@

$(TEST_ASAN_BIN): $(BIN_DIR)/$(TARGET) $(TEST_SRC) include/libft.h tests/test.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $(ASAN_FLAGS) $(TEST_SRC) $(BIN_DIR)/$(TARGET) -o $@

clean:
	rm -rf $(BIN_DIR)

re: clean all

-include $(DEPS)