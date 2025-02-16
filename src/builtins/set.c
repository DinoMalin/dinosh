#include "builtins.h"

void just_list(Env *env) {
	for (Env *curr = env; curr; curr=curr->next) {
		if (curr->type & INTERN) {
			ft_putstr_fd(env->var, 1);
			ft_putstr_fd("=", 1);
			ft_putstr_fd(env->value, 1);
			ft_putstr_fd(RESET"\n", 1);
		}
	}
}

void set(Command *cmd, Context *ctx) {
	if (cmd->ac == 1) {
		just_list(ctx->env);
		return;
	}

	int durability = -1;
	for (int i = 0; cmd->av[i]; i++) {
		if (!ft_strcmp(cmd->av[i], "-d")) {
			i++;
			if (!cmd->av[i])
				BUILTIN_ERROR("set: missing parameter");
			if (!is_number(cmd->av[i]))
				BUILTIN_ERROR("set: numeric argument required");
			durability = ft_atoi(cmd->av[i]) + 1;
		}
	}

	for (int i = 0; cmd->av[i]; i++) {
		char *content = ft_strchr(cmd->av[i], '=');
		if (content) {
			char *var = ft_substr(cmd->av[i], 0, content - cmd->av[i]);
			if (!var_is_valid(var)) {
				free(var);
				BUILTIN_ERROR("set: not a valid identifier");
			}
			modify_env(&ctx->env, var, content + 1, INTERN, durability);
			free(var);
		}
	}
}
