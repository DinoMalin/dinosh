#include "minishell.h"

bool has_n_flag(Command *cmd) {
	if (cmd->ac < 2)
		return false;
	if (cmd->av[1][0] != '-')
		return false;

	bool res = false;
	for (int i = 1; cmd->av[1][i]; i++) {
		if (cmd->av[1][i] != 'n')
			return false;
		res = true;
	}
	return res;
}

int echo(Command *cmd) {
	int n_flag = has_n_flag(cmd);

	for (int i = n_flag ? 2 : 1; cmd->av[i]; i++) {
		printf("%s", cmd->av[i]);
		if (i < cmd->ac - 1)
			printf(" ");
	}

	if (!n_flag) {
		printf("\n");
	}
		
	return 1;
}
