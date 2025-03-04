#include "push_swap.h"

int	apply_checker_command(const char *line, t_stack *a, t_stack *b)
{
	if (ps_strcmp(line, "sa") == 0)
		return (op_sa(a, 0));
	else if (ps_strcmp(line, "sb") == 0)
		return (op_sb(b, 0));
	else if (ps_strcmp(line, "ss") == 0)
		return (op_ss(a, b, 0));
	else if (ps_strcmp(line, "pa") == 0)
		return (op_pa(a, b, 0));
	else if (ps_strcmp(line, "pb") == 0)
		return (op_pb(a, b, 0));
	else if (ps_strcmp(line, "ra") == 0)
		return (op_ra(a, 0));
	else if (ps_strcmp(line, "rb") == 0)
		return (op_rb(b, 0));
	else if (ps_strcmp(line, "rr") == 0)
		return (op_rr(a, b, 0));
	else if (ps_strcmp(line, "rra") == 0)
		return (op_rra(a, 0));
	else if (ps_strcmp(line, "rrb") == 0)
		return (op_rrb(b, 0));
	else if (ps_strcmp(line, "rrr") == 0)
		return (op_rrr(a, b, 0));
	return (0);
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
	int		status;

	if (argc == 1)
		return (ps_test_finish(0));
	if (!ps_ignore_sigpipe())
		return (write_error(), ps_test_finish(1));
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
	status = 0;
	if (stack_is_complete_sorted(&a, &b))
		status = !ps_putstr_fd(1, "OK\n");
	else
		status = !ps_putstr_fd(1, "KO\n");
	stack_free(&a);
	stack_free(&b);
	if (status != 0)
		write_error();
	return (ps_test_finish(status));
}
