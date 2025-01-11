#include "builtins.h"

void unset(Command *cmd, Context *ctx) {
	if (cmd->ac < 2) {
		dprintf(2, "dinosh: not enough args\n");
		return;
	}
	for (int i = 0; cmd->av[i]; i++) {
		delete_var(ctx->env, cmd->av[i]);
	}
}
