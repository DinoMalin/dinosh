#include "builtins.h"

void type(Command *cmd, Context *ctx) {
	if (cmd->ac == 1)
		BUILTIN_ERROR("type: not enough arg");

	for (int i = 1; cmd->av[i]; i++) {
		if (get_builtin(cmd->av[i])) {
			ft_putstr_fd(cmd->av[i], 1);
			ft_putstr_fd(" is a shell builtin\n", 1);
			continue;
		}

		char *path = find_path(ctx, cmd->av[i]);
		if (!path)
			BUILTIN_ERROR("%s: not found", cmd->av[i]);

		ft_putstr_fd(cmd->av[i], 1);
		ft_putstr_fd(" is ", 1);
		ft_putstr_fd(path, 1);
		ft_putstr_fd("\n", 1);
		free(path);
	}
}
