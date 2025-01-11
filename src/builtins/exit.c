#include "builtins.h"

void builtin_exit(Command *cmd, Context *ctx) {
	if (cmd->ac > 1) {
		dprintf(2, "dinosh: too many args\n");
		return;
	}
	ctx->exit = true;
}
