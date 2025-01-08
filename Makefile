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
	src/char/ft_char.c
OBJ_DIR := build/obj
OBJ := $(SRC:%.c=$(OBJ_DIR)/%.o)
DEP := $(OBJ:.o=.d)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

$(OBJ_DIR)/%.o: %.c libft.h
	@$(MKDIR) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	$(RMDIR) build

fclean: clean
	$(RM) $(NAME)

re: fclean all

-include $(DEP)
