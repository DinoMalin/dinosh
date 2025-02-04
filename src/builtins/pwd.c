#include "builtins.h"

void pwd(Command *cmd) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("pwd: too many args");

	char *pwd = getcwd(NULL, 0);
	ft_putstr_fd(pwd, 1);
	ft_putstr_fd("\n", 1);
	free(pwd);
}
