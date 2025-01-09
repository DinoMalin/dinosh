#include "minishell.h"

int builtin(Command *cmd) {
	if (cmd->type == ECHO)
		return echo(cmd);
	return 0;
}
