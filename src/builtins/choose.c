#include "builtins.h"

void builtin(Command *cmd, Context *ctx) {
	if (cmd->type == ECHO)
		echo(cmd);
	else if (cmd->type == CD)
		cd(cmd, ctx);
	else if (cmd->type == PWD)
		pwd(cmd);
	else if (cmd->type == EXPORT)
		export(cmd, ctx);
	else if (cmd->type == UNSET)
		unset(cmd, ctx);
	else if (cmd->type == ENV)
		env(cmd, ctx->env);
	else if (cmd->type == EXIT)
		builtin_exit(cmd, ctx);
	else if (cmd->type == SET)
		set(cmd, ctx);
}
