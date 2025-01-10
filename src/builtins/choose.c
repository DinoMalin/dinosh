#include "minishell.h"

void builtin(Command *cmd, Prompt *prompt) {
	if (cmd->type == ECHO)
		echo(cmd);
	else if (cmd->type == CD)
		cd(cmd, prompt->env);
	else if (cmd->type == PWD)
		pwd(cmd);
	else if (cmd->type == EXPORT)
		export(cmd, prompt);
	else if (cmd->type == UNSET)
		unset(cmd, prompt);
	else if (cmd->type == ENV)
		env(cmd, prompt->env);
}
