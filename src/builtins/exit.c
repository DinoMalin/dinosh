#include "builtins.h"

void builtin_exit(Command *cmd, Context *ctx) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("too many args");

	ctx->exit = true;
}
