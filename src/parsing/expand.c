#include "parse.h"

static char *ft_getenv_alloc(char **envp, char *target) {
	char *res = ft_getenv(envp, target);
	if (!res)
		return ft_strdup("");
	return ft_strdup(res);
}

int get_variable(char *str) {
	ADD_SPECIAL_VAR("?");

	int size;
	for (size = 0; str[size]; size++) {
		if (!	((str[size] >= 'A' && str[size] <= 'Z')
			||	(str[size] >= '0' && str[size] <= '9')
			||	str[size] == '_'))
			break;
	}
	return size;
}

char *extract(char *dest, char *src, int size) {
	char *sub = ft_substr(src, 0, size);
	dest = clean_join(dest, sub);
	free(sub);
	return dest;
}

char *expand(char *str, char **envp) {
	char	*result = ft_strdup("");
	char	*variable = NULL;
	char	*translated_var = NULL;
	int		size_var = 0;
	int		i;

	int start_copy = 0;
	for (i = 0; str[i]; i++) {
		if (str[i] == '$' && str[i + 1]) {
			if (i != 0)
				result = extract(result, str + start_copy, i - start_copy);
			size_var = get_variable(str + i + 1);
			variable = ft_substr(str, i + 1, size_var);
			translated_var = ft_getenv_alloc(envp, variable);
			free(variable);
			result = clean_join(result, translated_var);
			free(translated_var);
			i += size_var + 1;
			start_copy = i;
		}
		if (!str[i])
			break;
	}
	if (str[start_copy])
		result = extract(result, str + start_copy, i - start_copy);

	return result;
}

