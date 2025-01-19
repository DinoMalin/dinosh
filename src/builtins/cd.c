#include "builtins.h"

void update_pwd(char *old, Context *ctx) {
	char *new = getcwd(NULL, 0);

	ctx->env = modify_env(ctx->env, "PWD", new);
	ctx->env = modify_env(ctx->env, "OLDPWD", old);

	free(new);
	free(old);
}

void cd(Command *cmd, Context *ctx) {
	if (cmd->ac > 2)
		BUILTIN_ERROR("too many args");

	char *oldpwd = getcwd(NULL, 0);
	if (cmd->ac < 2) {
		char *home = ft_getenv(ctx->env, "HOME");
		if (!home) {
			free(oldpwd);
			BUILTIN_ERROR("not enough args");
		}
		if (chdir(home) == -1) {
			free(oldpwd);
			BUILTIN_PERROR("cd");
		}
		update_pwd(oldpwd, ctx);
		return;
	}
	if (chdir(cmd->av[1]) == -1) {
		free(oldpwd);
		BUILTIN_PERROR("cd");
	}
	update_pwd(oldpwd, ctx);
}
