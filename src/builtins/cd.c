#include "minishell.h"

void cd(Command *cmd, char **env) {
	if (cmd->ac > 2) {
		dprintf(2, "dinosh: too many args\n");
		return;
	}

	if (cmd->ac < 2) {
		char *home = ft_getenv(env, "HOME");
		if (!home) {
			dprintf(2, "dinosh: not enough args\n");
			return;
		}
		if (chdir(home) == -1) {
			perror("dinosh: cd");
			return;
		}
		return;
	}
	if (chdir(cmd->av[1]) == -1) {
		perror("dinosh: cd");
		return;
	}
}
