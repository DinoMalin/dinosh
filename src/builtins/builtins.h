#include "minishell.h"

#define BUILTIN(n, func)			\
	{								\
		if (cmd->type == n) {		\
			func(cmd, ctx);			\
			return;					\
		}							\
	}

#define BUILTIN_ERROR(format, ...)			\
	{										\
		ERROR(format, ##__VA_ARGS__);			\
		cmd->exit_code = 1;					\
		return;								\
	}

#define BUILTIN_PERROR(msg)				\
	{									\
		perror("dinosh: "msg);			\
		cmd->exit_code = 1;				\
		return;							\
	}

#define RESET "\033[0m"

void	echo(Command *cmd, Context *ctx);
void	cd(Command *cmd, Context *ctx);
void	pwd(Command *cmd, Context *ctx);
void	export(Command *cmd, Context *ctx);
void	unset(Command *cmd, Context *ctx);
void	env(Command *cmd, Context *ctx);
void	builtin_exit(Command *cmd, Context *ctx);
void	set(Command *cmd, Context *ctx);
void	type(Command *cmd, Context *ctx);
void	jobs(Command *cmd, Context *ctx);
void	fg(Command *cmd, Context *ctx);
void	bg(Command *cmd, Context *ctx);
void	hash(Command *cmd, Context *ctx);
void	test(Command *cmd, Context *ctx);
void	alias(Command *cmd, Context *ctx);
void	unalias(Command *cmd, Context *ctx);
void	fc(Command *cmd, Context *ctx);
