#include "minishell.h"

void unset(Command *cmd, Prompt *prompt) {
	if (cmd->ac < 2) {
		dprintf(2, "dinosh: not enough args\n");
		return;
	}
	for (int i = 0; cmd->av[i]; i++) {
		delete_var(prompt->env, cmd->av[i]);
	}
}
