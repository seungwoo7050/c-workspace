#include "push_swap.h"

int	apply_checker_command(const char *line, t_stack *a, t_stack *b)
{
	if (ps_strcmp(line, "sa") == 0)
		op_sa(a, 0);
	else if (ps_strcmp(line, "sb") == 0)
		op_sb(b, 0);
	else if (ps_strcmp(line, "ss") == 0)
		op_ss(a, b, 0);
	else if (ps_strcmp(line, "pa") == 0)
		op_pa(a, b, 0);
	else if (ps_strcmp(line, "pb") == 0)
		op_pb(a, b, 0);
	else if (ps_strcmp(line, "ra") == 0)
		op_ra(a, 0);
	else if (ps_strcmp(line, "rb") == 0)
		op_rb(b, 0);
	else if (ps_strcmp(line, "rr") == 0)
		op_rr(a, b, 0);
	else if (ps_strcmp(line, "rra") == 0)
		op_rra(a, 0);
	else if (ps_strcmp(line, "rrb") == 0)
		op_rrb(b, 0);
	else if (ps_strcmp(line, "rrr") == 0)
		op_rrr(a, b, 0);
	else
		return (0);
	return (1);
}

static int	read_and_apply(t_stack *a, t_stack *b)
{
	char	*line;
	int		status;

	status = read_next_line(0, &line);
	while (status > 0)
	{
		if (!apply_checker_command(line, a, b))
		{
			ps_free(line);
			return (0);
		}
		ps_free(line);
		status = read_next_line(0, &line);
	}
	return (status == 0);
}

int	main(int argc, char **argv)
{
	t_stack	a;
	t_stack	b;

	if (argc == 1)
		return (ps_test_finish(0));
	if (!parse_input(argc, argv, &a))
		return (write_error(), ps_test_finish(1));
	if (!stack_init(&b, a.capacity))
	{
		stack_free(&a);
		return (write_error(), ps_test_finish(1));
	}
	if (!read_and_apply(&a, &b))
	{
		stack_free(&a);
		stack_free(&b);
		return (write_error(), ps_test_finish(1));
	}
	if (stack_is_complete_sorted(&a, &b))
		ps_putstr_fd(1, "OK\n");
	else
		ps_putstr_fd(1, "KO\n");
	stack_free(&a);
	stack_free(&b);
	return (ps_test_finish(0));
}
