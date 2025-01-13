#include "test.h"

#include "libft.h"

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

static void	check_output(void)
{
	static const char	expected[] =
		"Afoundation\n0|-1|2147483647|-2147483648";
	char			actual[sizeof(expected) + 8];
	int			pipe_fd[2];
	ssize_t			bytes_read;

	CHECK(pipe(pipe_fd) == 0);
	ft_putchar_fd('A', pipe_fd[1]);
	ft_putendl_fd("foundation", pipe_fd[1]);
	ft_putnbr_fd(0, pipe_fd[1]);
	ft_putchar_fd('|', pipe_fd[1]);
	ft_putnbr_fd(-1, pipe_fd[1]);
	ft_putchar_fd('|', pipe_fd[1]);
	ft_putnbr_fd(INT_MAX, pipe_fd[1]);
	ft_putchar_fd('|', pipe_fd[1]);
	ft_putnbr_fd(INT_MIN, pipe_fd[1]);
	CHECK(close(pipe_fd[1]) == 0);
	bytes_read = read(pipe_fd[0], actual, sizeof(actual));
	CHECK(bytes_read == (ssize_t)(sizeof(expected) - 1));
	if (bytes_read == (ssize_t)(sizeof(expected) - 1))
		CHECK(memcmp(actual, expected, sizeof(expected) - 1) == 0);
	CHECK(close(pipe_fd[0]) == 0);
}

static void	check_broken_pipe_policy(void)
{
	void	(*previous_handler)(int);
	int		pipe_fd[2];

	previous_handler = signal(SIGPIPE, SIG_IGN);
	CHECK(previous_handler != SIG_ERR);
	CHECK(pipe(pipe_fd) == 0);
	CHECK(close(pipe_fd[0]) == 0);
	errno = 0;
	ft_putstr_fd("closed", pipe_fd[1]);
	CHECK(errno == EPIPE);
	CHECK(close(pipe_fd[1]) == 0);
	CHECK(signal(SIGPIPE, previous_handler) == SIG_IGN);
}

void	test_fd_output(void)
{
	int	pipe_fd[2];

	check_output();
	check_broken_pipe_policy();
	CHECK(pipe(pipe_fd) == 0);
	CHECK(close(pipe_fd[0]) == 0);
	CHECK(close(pipe_fd[1]) == 0);
	ft_putchar_fd('x', pipe_fd[1]);
	ft_putstr_fd("", pipe_fd[1]);
	ft_putendl_fd("", pipe_fd[1]);
	ft_putnbr_fd(42, pipe_fd[1]);
}
