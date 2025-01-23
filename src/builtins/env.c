#include "builtins.h"

void env(Command *cmd, char **env) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("env: too many args");

	for (int i = 0; env[i]; i++) {
		printf("%s\n", env[i]);
	}
}
