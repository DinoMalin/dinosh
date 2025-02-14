#include "minishell.h"

bool is_number(char *str) {
	if (!*str)
		return false;

	if (*str == '+' || *str == '-' || ft_isdigit(*str))
		str++;
	else
		return false;

	while (*str) {
		if (!ft_isdigit(*str))
			return false;
		str++;
	}

	return true;
}

