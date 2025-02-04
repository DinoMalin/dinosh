#include "builtins.h"

#define RESET "\033[0m"

#define SKIP_VAR(s)										\
	{													\
		char *e = ft_strchr(env[i], '=');				\
		if (e && !ft_strncmp(env[i], s, e - env[i]))	\
			continue;									\
	}

void env(Command *cmd, Env *env) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("env: too many args");

	while (env) {
		if (!(env->type & SPECIAL)) {
			ft_putstr_fd(env->var, 1);
			ft_putstr_fd("=", 1);
			ft_putstr_fd(env->value, 1);
			ft_putstr_fd(RESET"\n", 1);
		}
		env = env->next;
	}
}
