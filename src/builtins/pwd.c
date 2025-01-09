#include "minishell.h"

void pwd(Command *cmd) {
	if (cmd->ac > 1) {
		dprintf(2, "dinosh: too many args\n");
		return;
	}

	char *pwd = getcwd(NULL, 0);
	printf("%s\n", pwd);
	free(pwd);
}
