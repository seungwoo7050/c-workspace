#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include <stddef.h>
# include <stdlib.h>
# include <unistd.h>

typedef struct s_stack
{
	int	*values;
	int	*ranks;
	int	size;
	int	capacity;
}	t_stack;

void	stack_init_empty(t_stack *stack);
int		stack_init(t_stack *stack, int capacity);
void	stack_free(t_stack *stack);
int		stack_is_sorted(const t_stack *stack);
int		stack_is_complete_sorted(const t_stack *a, const t_stack *b);

void	stack_swap(t_stack *stack);

void	op_sa(t_stack *a, int emit);
void	op_sb(t_stack *b, int emit);
void	op_ss(t_stack *a, t_stack *b, int emit);

size_t	ps_strlen(const char *str);
int		ps_strcmp(const char *a, const char *b);
void	ps_putstr_fd(int fd, const char *str);
void	write_error(void);

#endif
