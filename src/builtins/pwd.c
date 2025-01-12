#include "builtins.h"

void pwd(Command *cmd) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("too many args");

	char *pwd = getcwd(NULL, 0);
	printf("%s\n", pwd);
	free(pwd);
}
