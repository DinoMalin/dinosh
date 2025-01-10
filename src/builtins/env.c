#include "minishell.h"

void env(Command *cmd, char **env) {
	if (cmd->ac > 1) {
		dprintf(2, "dinosh: too many args\n");
		return;
	}
	for (int i = 0; env[i]; i++) {
		printf("%s\n", env[i]);
	}
}
