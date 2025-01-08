#ifndef TEST_H
# define TEST_H

void	test_check(int condition, const char *expression, const char *file,
			int line);
int		test_finish(void);
void	test_char(void);
void	test_memory_fill(void);
void	test_memory_copy(void);

# define CHECK(expression) \
	test_check((expression), #expression, __FILE__, __LINE__)

#endif
