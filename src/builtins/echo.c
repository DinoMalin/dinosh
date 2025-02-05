#include "builtins.h"

void echo(Command *cmd) {
	bool nflag = false;
	int i = 1;
	while (cmd->av[i] && !ft_strncmp(cmd->av[i], "-n", 2)) {
		nflag = true;
		i++;
	}

	for (; cmd->av[i]; i++) {
		ft_putstr_fd(cmd->av[i], 1);
		if (i < cmd->ac - 1)
			ft_putstr_fd(" ", 1);
	}

	if (!nflag) {
		ft_putstr_fd("\n", 1);
	}
}
