#include "minishell.h"

void builtin(Command *cmd, char **env) {
	if (cmd->type == ECHO)
		echo(cmd);
	else if (cmd->type == CD)
		cd(cmd, env);
}
