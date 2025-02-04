#include "builtins.h"

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

void echo(Command *cmd) {
	int n_flag = has_n_flag(cmd);

	for (int i = n_flag ? 2 : 1; cmd->av[i]; i++) {
		ft_putstr_fd(cmd->av[i], 1);
		if (i < cmd->ac - 1)
			ft_putstr_fd(" ", 1);
	}

	if (!n_flag) {
		ft_putstr_fd("\n", 1);
	}
}
