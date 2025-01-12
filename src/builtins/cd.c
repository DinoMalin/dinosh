#include "builtins.h"

void cd(Command *cmd, char **env) {
	if (cmd->ac > 2)
		BUILTIN_ERROR("too many args");

	if (cmd->ac < 2) {
		char *home = ft_getenv(env, "HOME");
		if (!home)
			BUILTIN_ERROR("not enough args");
		if (chdir(home) == -1)
			BUILTIN_PERROR("cd");
		return;
	}
	if (chdir(cmd->av[1]) == -1)
		BUILTIN_PERROR("cd");
}
