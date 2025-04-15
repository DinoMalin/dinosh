#include "builtins.h"

#define ARG(n) reverse ? n+1 : n

void test(Command *cmd, Context *ctx) {
	(void)ctx;
	bool reverse = cmd->ac > 2 && !ft_strcmp(cmd->av[1], "!");
	bool resolved = false;

	if (cmd->ac == ARG(3) && !ft_strcmp(cmd->av[ARG(1)], "-z")) {
		cmd->exit_code = ft_strlen(cmd->av[ARG(2)]) > 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "=")) {
		cmd->exit_code = !!ft_strcmp(cmd->av[ARG(1)], cmd->av[ARG(3)]);
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "!=")) {
		cmd->exit_code = !ft_strcmp(cmd->av[ARG(1)], cmd->av[ARG(3)]);
		resolved = true;
	}


	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-eq")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) == ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-ne")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) != ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-ge")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) >= ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-gt")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) > ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-le")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) <= ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-lt")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) < ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}

	if (resolved && reverse)
		cmd->exit_code = !cmd->exit_code;

	if (!resolved) {
		BUILTIN_ERROR("condition expected");
	}
}
