#include "push_swap.h"

#include <stdio.h>

typedef struct s_fixture
{
	t_stack	a;
	t_stack	b;
}	t_fixture;

static int	expected_rank(int value)
{
	if (value == 40)
		return (4);
	if (value == 10)
		return (1);
	if (value == 30)
		return (3);
	if (value == 20)
		return (2);
	if (value == 0)
		return (0);
	return (-1);
}

static int	stack_pairs_are_valid(const t_stack *stack)
{
	int	i;

	if (stack->size < 0 || stack->size > stack->capacity)
		return (0);
	i = 0;
	while (i < stack->size)
	{
		if (expected_rank(stack->values[i]) != stack->ranks[i])
			return (0);
		i++;
	}
	return (1);
}

static int	all_pairs_are_present(const t_fixture *fixture)
{
	int	seen[5];
	int	i;
	int	rank;

	i = 0;
	while (i < 5)
		seen[i++] = 0;
	i = 0;
	while (i < fixture->a.size)
	{
		rank = fixture->a.ranks[i++];
		if (rank < 0 || rank >= 5 || seen[rank])
			return (0);
		seen[rank] = 1;
	}
	i = 0;
	while (i < fixture->b.size)
	{
		rank = fixture->b.ranks[i++];
		if (rank < 0 || rank >= 5 || seen[rank])
			return (0);
		seen[rank] = 1;
	}
	i = 0;
	while (i < 5)
	{
		if (!seen[i++])
			return (0);
	}
	return (1);
}

static int	fixture_init(t_fixture *fixture)
{
	if (!stack_init(&fixture->a, 5))
		return (0);
	if (!stack_init(&fixture->b, 5))
	{
		stack_free(&fixture->a);
		return (0);
	}
	fixture->a.values[0] = 40;
	fixture->a.ranks[0] = 4;
	fixture->a.values[1] = 10;
	fixture->a.ranks[1] = 1;
	fixture->a.values[2] = 30;
	fixture->a.ranks[2] = 3;
	fixture->a.size = 3;
	fixture->b.values[0] = 20;
	fixture->b.ranks[0] = 2;
	fixture->b.values[1] = 0;
	fixture->b.ranks[1] = 0;
	fixture->b.size = 2;
	return (1);
}

static void	fixture_free(t_fixture *fixture)
{
	stack_free(&fixture->a);
	stack_free(&fixture->b);
}

static int	fixture_is_valid(const char *operation, const t_fixture *fixture)
{
	if (!stack_pairs_are_valid(&fixture->a)
		|| !stack_pairs_are_valid(&fixture->b)
		|| fixture->a.size + fixture->b.size != 5
		|| !all_pairs_are_present(fixture))
	{
		fprintf(stderr, "%s broke the value/rank pairing invariant\n",
			operation);
		return (0);
	}
	return (1);
}

static int	check_fixture(const char *operation, t_fixture *fixture)
{
	int	valid;

	valid = fixture_is_valid(operation, fixture);
	fixture_free(fixture);
	return (valid);
}

static int	test_operation_sequence(void)
{
	t_fixture	fixture;

	if (!fixture_init(&fixture))
		return (0);
#define APPLY_AND_CHECK(name, call) \
	do { \
		call; \
		if (!fixture_is_valid(name " in sequence", &fixture)) \
			return (fixture_free(&fixture), 0); \
	} while (0)
	APPLY_AND_CHECK("sa", op_sa(&fixture.a, 0));
	APPLY_AND_CHECK("sb", op_sb(&fixture.b, 0));
	APPLY_AND_CHECK("ss", op_ss(&fixture.a, &fixture.b, 0));
	APPLY_AND_CHECK("pa", op_pa(&fixture.a, &fixture.b, 0));
	APPLY_AND_CHECK("pb", op_pb(&fixture.a, &fixture.b, 0));
	APPLY_AND_CHECK("ra", op_ra(&fixture.a, 0));
	APPLY_AND_CHECK("rb", op_rb(&fixture.b, 0));
	APPLY_AND_CHECK("rr", op_rr(&fixture.a, &fixture.b, 0));
	APPLY_AND_CHECK("rra", op_rra(&fixture.a, 0));
	APPLY_AND_CHECK("rrb", op_rrb(&fixture.b, 0));
	APPLY_AND_CHECK("rrr", op_rrr(&fixture.a, &fixture.b, 0));
#undef APPLY_AND_CHECK
	fixture_free(&fixture);
	return (1);
}

static int	test_operations(void)
{
	t_fixture	fixture;

#define RUN_OPERATION(name, call) \
	do { \
		if (!fixture_init(&fixture)) \
			return (0); \
		call; \
		if (!check_fixture(name, &fixture)) \
			return (0); \
	} while (0)
	RUN_OPERATION("sa", op_sa(&fixture.a, 0));
	RUN_OPERATION("sb", op_sb(&fixture.b, 0));
	RUN_OPERATION("ss", op_ss(&fixture.a, &fixture.b, 0));
	RUN_OPERATION("pa", op_pa(&fixture.a, &fixture.b, 0));
	RUN_OPERATION("pb", op_pb(&fixture.a, &fixture.b, 0));
	RUN_OPERATION("ra", op_ra(&fixture.a, 0));
	RUN_OPERATION("rb", op_rb(&fixture.b, 0));
	RUN_OPERATION("rr", op_rr(&fixture.a, &fixture.b, 0));
	RUN_OPERATION("rra", op_rra(&fixture.a, 0));
	RUN_OPERATION("rrb", op_rrb(&fixture.b, 0));
	RUN_OPERATION("rrr", op_rrr(&fixture.a, &fixture.b, 0));
#undef RUN_OPERATION
	return (1);
}

int	main(void)
{
	if (!test_operations() || !test_operation_sequence())
		return (1);
	printf("operation pairing invariants passed\n");
	return (0);
}
