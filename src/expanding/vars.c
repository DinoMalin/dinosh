#include "expand.h"

char *ft_getenv_alloc(Env *env, char *target) {
	char *res = ft_getenv(env, target);
	if (!res)
		return ft_strdup("");
	return ft_strdup(res);
}

void var(Env *env, Parser *el, int max) {
	char *value = ft_getenv_alloc(env, el->content);
	if (el->quoting == doubles) {
		free(el->content);
		el->content = value;
	} else {
		add_tokenized_args(el, value, max);
		free(value);
	}
}
