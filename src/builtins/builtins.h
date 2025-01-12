#include "minishell.h"

void	delete_var(char **env, char *var);
void	echo(Command *cmd);
void	cd(Command *cmd, char **env);
void	pwd(Command *cmd);
void	export(Command *cmd, Context *ctx);
void	unset(Command *cmd, Context *ctx);
void	env(Command *cmd, char **env);
void	builtin_exit(Command *cmd, Context *ctx);
