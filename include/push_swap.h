#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include <stddef.h>
# include <stdlib.h>

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

#endif
