#include "expand.h"

char *do_op(char *str) {
	(void)str;
	return ft_strdup("[todo: arithmetic]");
}

void arithmetic(Env *env, Parser *el) {
	(void)env;
	char *s = el->content;
	el->content = do_op(el->content);
	free(s);
}
