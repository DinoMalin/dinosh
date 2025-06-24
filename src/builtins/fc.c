#include "builtins.h"

void fc(Command *cmd, Context *ctx) {
	(void)ctx;
	if (cmd->ac > 2)
		BUILTIN_ERROR("fc: too many args");
}
