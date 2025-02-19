#include "parsing.h"

Type get_builtin(char *name) {
	if (!ft_strcmp("echo", name))
		return ECHO;
	if (!ft_strcmp("cd", name))
		return CD;
	if (!ft_strcmp("pwd", name))
		return PWD;
	if (!ft_strcmp("export", name))
		return EXPORT;
	if (!ft_strcmp("unset", name))
		return UNSET;
	if (!ft_strcmp("env", name))
		return ENV;
	if (!ft_strcmp("exit", name))
		return EXIT;
	if (!ft_strcmp("set", name))
		return SET;
	if (!ft_strcmp("type", name))
		return TYPE;
	if (!ft_strcmp("jobs", name))
		return JOBS;
	if (!ft_strcmp("fg", name))
		return FG;
	if (!ft_strcmp("bg", name))
		return BG;
	return BASIC;
}

void check_type(Command *cmd, Token token) {
	char *name = cmd->args->content;

	if (token == t_subshell)
		cmd->type = SUBSHELL;
	Type builtin = get_builtin(name);
	if (builtin)
		cmd->type = builtin;
}

void check_redir_errors(Command *cmd, Parser **data) {
	Parser *file = (*data)->next;

	if (!file) {
		cmd->error = empty_redir;
		return;
	}
	if (!CAN_REDIR(file->token)) {
		cmd->error = unexpected_token;
		return;
	}
}
