#include "builtins.h"

void test(Command *cmd, Context *ctx) {
	(void)ctx;

	if (cmd->ac == 3 && !ft_strcmp(cmd->av[1], "-z")) {
		cmd->exit_code = ft_strlen(cmd->av[2]) > 0;
		return;
	}
	if (cmd->ac == 4 && !ft_strcmp(cmd->av[2], "=")) {
		cmd->exit_code = !!ft_strcmp(cmd->av[1], cmd->av[3]);
		return;
	}
	if (cmd->ac == 4 && !ft_strcmp(cmd->av[2], "!=")) {
		cmd->exit_code = !ft_strcmp(cmd->av[1], cmd->av[3]);
		return;
	}

	BUILTIN_ERROR("condition expected");
}
