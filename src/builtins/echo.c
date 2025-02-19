#include "builtins.h"

void echo(Command *cmd, Context *ctx) {
	(void)ctx;
	bool nflag = false;
	int i = 1;

	while (cmd->av[i] && !ft_strncmp(cmd->av[i], "-n", 2)) {
		nflag = true;
		i++;
	}

	char *to_print = ft_strdup("");
	for (; cmd->av[i]; i++) {
		to_print = clean_join(to_print, cmd->av[i]);
		if (i < cmd->ac - 1)
			to_print = clean_join(to_print, " ");
	}

	if (!nflag) {
		to_print = clean_join(to_print, "\n");
	}

	ft_putstr_fd(to_print, 1);
	free(to_print);
}
