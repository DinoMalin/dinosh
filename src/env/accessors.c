#include "minishell.h"

char *ft_getenv(Env *env, char *target) {
	while (env) {
		if (!ft_strcmp(target, env->var))
			return env->value;
		env = env->next;
	}
	return NULL;
}

Env *getvar(Env *env, char *target) {
	while (env) {
		if (!ft_strcmp(target, env->var))
			return env;
		env = env->next;
	}
	return NULL;
}

void set_extern(Env *env, char *target) {
	while (env) {
		if (!ft_strcmp(target, env->var))
			env->type = EXTERN;
		env = env->next;
	}
}

char **get_envp(Env *env) {
	int size = 0;
	Env *curr = env;
	while (curr) {
		if (!(curr->type & INTERN || curr->type & SPECIAL))
			size++;
		curr = curr->next;
	}

	char **res = malloc((size + 1) * sizeof(char *));
	res[size] = NULL;

	int i = 0;
	while (env) {
		if (!(env->type & INTERN || env->type & SPECIAL)) {
			char *new = ft_strdup(env->var);
			new = clean_join(new, "=");
			new = clean_join(new, env->value);
			res[i] = new;

			i++;
		}
		env = env->next;
	}
	return res;
}
