#include "minishell.h"

char **strsjoin(char **origin, char *str) {
	int len = 0;

	while (origin[len]) {
		len++;
	}
	
	char **res = malloc((len + 2) * sizeof(char *));
	res[len + 1] = NULL;
	res[len] = str;
	while (len) {
		res[len - 1] = origin[len - 1];
		len--;
	}

	return res;
}

t_redir *redirjoin(t_redir *origin, t_redir r) {
	int len = 0;

	while (origin[len].file) {
		len++;
	}
	
	t_redir *res = malloc((len + 2) * sizeof(t_redir));
	res[len + 1] = (t_redir){NULL, 0};
	res[len] = r;
	while (len) {
		res[len - 1] = origin[len - 1];
		len--;
	}

	return res;
}

char **clean_strsjoin(char **origin, char *to_join) {
	char **res = strsjoin(origin, to_join);
	free(origin);
	return res;
}

char *clean_join(char *origin, const char *to_join) {
	char *res = ft_strjoin(origin, to_join);
	free(origin);
	return res;
}

t_redir *clean_redirjoin(t_redir *origin, t_redir to_join) {
	t_redir *res = redirjoin(origin, to_join);
	free(origin);
	return res;
}
