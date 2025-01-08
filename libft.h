#ifndef LIBFT_H
# define LIBFT_H

# include <stddef.h>

int	ft_isalpha(int c);
int	ft_isdigit(int c);
int	ft_isalnum(int c);
int	ft_isascii(int c);
int	ft_isprint(int c);
int	ft_toupper(int c);
int	ft_tolower(int c);

void	*ft_memset(void *memory, int byte, size_t length);
void	ft_bzero(void *memory, size_t length);

#endif
