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
void	*ft_memcpy(void *destination, const void *source, size_t length);
void	*ft_memmove(void *destination, const void *source, size_t length);
void	*ft_memchr(const void *memory, int byte, size_t length);
int		ft_memcmp(const void *left, const void *right, size_t length);

size_t	ft_strlen(const char *text);
size_t	ft_strlcpy(char *destination, const char *source, size_t capacity);
size_t	ft_strlcat(char *destination, const char *source, size_t capacity);
char	*ft_strchr(const char *text, int character);
char	*ft_strrchr(const char *text, int character);
int		ft_strncmp(const char *left, const char *right, size_t length);
char	*ft_strnstr(const char *haystack, const char *needle, size_t length);
int		ft_atoi(const char *text);

void	*ft_calloc(size_t count, size_t size);
char	*ft_strdup(const char *text);
char	*ft_substr(const char *text, unsigned int start, size_t length);
char	*ft_strjoin(const char *left, const char *right);
char	*ft_strtrim(const char *text, const char *set);
char	**ft_split(const char *text, char delimiter);

#endif
