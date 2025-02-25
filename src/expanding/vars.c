#include "expand.h"

char *ft_getenv_alloc(Env *env, char *target) {
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

void expand_vars(Env *env, Parser *el, int max) {
	char *str = el->content;
	char *str_start = str;
	bool escape = false;
	el->content = ft_strdup("");
	Token token = el->token;

	char buff[B_SIZE] = {};
	int i = 0;

	while (*str) {
		if (*str == '$' && !escape) {
			el->content = clean_join(el->content, buff);
			bzero(buff, B_SIZE);
			i = 0;

			int var_size = get_variable(str+1);
			char *var = ft_substr(str+1, 0, var_size);
			char *value = ft_getenv_alloc(env, var);

			if (token == t_word)
				add_tokenized_args(el, value, max);
			else
				el->content = clean_join(el->content, value);

			free(var);
			free(value);
			str += var_size + 1;
		} else {
			if (*str == '\\') {
	   			escape = true;
			} else
				escape = false;

			if (ft_strncmp(str, "\\$", 2)) {
				buff[i] = *str;
				i++;
			}
			if (i+1 == B_SIZE) {
				el->content = clean_join(el->content, buff);
				bzero(buff, B_SIZE);
				i = 0;
			}
			str++;
		}
	}

	el->content = clean_join(el->content, buff);
	free(str_start);
}
