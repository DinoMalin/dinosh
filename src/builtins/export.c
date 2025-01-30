#include "builtins.h"

bool is_valid(char *name) {
	int i = 0;
	for (; name[i]; i++) {
		if (!(name[i] >= 'a' && name[i] <= 'z')
		 && !(name[i] >= 'A' && name[i] <= 'Z')
		 && !(name[i] >= '0' && name[i] <= '9' && i != 0)
		 && !(name[i] == '_')) {
			return false;
		}
	}
	if (!i)
		return false;
	return true;
}

void export(Command *cmd, Context *ctx) {
	if (cmd->ac < 2)
		BUILTIN_ERROR("export: not enough args");

	for (int i = 0; cmd->av[i]; i++) {
		char *content = ft_strchr(cmd->av[i], '=');
		if (content) {
			char *var = ft_substr(cmd->av[i], 0, content - cmd->av[i]);
			if (!is_valid(var)) {
				free(var);
				BUILTIN_ERROR("export: not a valid identifier");
			}
			modify_env(&ctx->env, var, content + 1);
			free(var);
		}
	}
}
