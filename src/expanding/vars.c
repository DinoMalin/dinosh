#include "expand.h"

static char *ft_getenv_alloc(Env *env, char *target) {
	char *res = ft_getenv(env, target);
	if (!res)
		return ft_strdup("");
	return ft_strdup(res);
}

int get_variable(char *str) {
	ADD_SPECIAL_VAR("?");

	int size;
	for (size = 0; str[size]; size++) {
		if (!(str[size] >= 'a' && str[size] <= 'z')
		 && !(str[size] >= 'A' && str[size] <= 'Z')
		 && !(str[size] >= '0' && str[size] <= '9' && size != 0)
		 && !(str[size] == '_')) {
			break;
		}
	}
	return size;
}

char *extract(char *dest, char *src, int size) {
	char *sub = ft_substr(src, 0, size);
	dest = clean_join(dest, sub);
	free(sub);

	return dest;
}

void expand_one_var(Env *env, Parser *el, int max) {
	char *value = ft_getenv_alloc(env, el->content);
	free(el->content);
	el->content = ft_strdup("");
	TOKENIZE_VAR();
	free(value);
}

void expand_vars(Env *env, Parser *el, int max) {
	int not_var = 0;
	char *str = el->content;
	char *str_start = str;
	el->content = ft_strdup("");
	Token token = el->token;

	while (*str) {
		if (*str == '$') {
			el->content = extract(el->content, str - not_var, not_var);

			int var_size = get_variable(str+1);
			char *var = ft_substr(str+1, 0, var_size);
			char *value = ft_getenv_alloc(env, var);

			if (token == t_word) {
				TOKENIZE_VAR();
			} else {
				el->content = clean_join(el->content, value);
			}

			free(var);
			free(value);
			str += var_size + 1;
			not_var = 0;
		} else {
			not_var++;
			str++;
		}
	}

	el->content = extract(el->content, str - not_var, not_var);
	free(str_start);
}
