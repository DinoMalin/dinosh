#include "minishell.h"

/* ====== UTILS ====== */
char	**modify_env(char **env, char *var, char *content);
void	delete_var(char **env, char *var);

/* ====== BUILTINS ====== */
void	echo(Command *cmd);
void	cd(Command *cmd, char **env);
void	pwd(Command *cmd);
void	export(Command *cmd, Context *ctx);
void	unset(Command *cmd, Context *ctx);
void	env(Command *cmd, char **env);
void	builtin_exit(Command *cmd, Context *ctx);
