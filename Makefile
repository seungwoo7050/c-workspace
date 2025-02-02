TARGET := libftprintf.a

CC         ?= cc
CFLAGS     ?= -std=c99 -Wall -Wextra -Werror -Iinclude -MMD -MP
AR         ?= ar
ARFLAGS    ?= rcs

SRC        := $(wildcard src/*.c)
BIN_DIR    := build
OBJS       := $(patsubst %.c,$(BIN_DIR)/%.o,$(SRC))
DEPS       := $(OBJS:.o=.d)
TEST_SRC   := $(wildcard tests/test_ft_*.c)
TEST_BIN   := $(BIN_DIR)/test/test_ft_printf
FAULT_SRC  := $(wildcard tests/test_output_*.c)
FAULT_BIN  := $(BIN_DIR)/test/test_output_faults

.PHONY: all clean fclean re test

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR):
	mkdir -p $@

$(BIN_DIR)/%.o: %.c include/ft_printf.h src/ft_printf_internal.h | $(BIN_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

$(TEST_BIN): $(TEST_SRC) $(BIN_DIR)/$(TARGET) include/ft_printf.h | $(BIN_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TEST_SRC) $(BIN_DIR)/$(TARGET) -o $@

$(FAULT_BIN): $(FAULT_SRC) $(SRC) include/ft_printf.h | $(BIN_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DFT_PRINTF_TEST_WRITE $(FAULT_SRC) $(SRC) -o $@

test: $(TEST_BIN) $(FAULT_BIN)
	@$(TEST_BIN)
	@$(FAULT_BIN)

clean:
	rm -rf $(BIN_DIR)

fclean: clean
	rm -f $(TARGET)

re: fclean all

-include $(DEPS)
