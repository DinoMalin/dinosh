#include "builtins.h"

#define BUILTIN_ERROR_CODE(msg, code)		\
	{										\
		ERROR(msg);							\
		cmd->exit_code = code;				\
		return;								\
	}

void builtin_exit(Command *cmd, Context *ctx) {
	if (cmd->ac > 2)
		BUILTIN_ERROR("exit: too many args");
	if (cmd->ac == 2) {
		if (!is_number(cmd->av[1]))
			BUILTIN_ERROR_CODE("exit: numeric argument required", 2);
		cmd->exit_code = (unsigned char)ft_atoi(cmd->av[1]);
	}

	ctx->exit = true;
}
