#include "parsing.h"

Type get_builtin(char *name) {
	BUILTIN("echo", ECHO);
	BUILTIN("cd", CD);
	BUILTIN("pwd", PWD);
	BUILTIN("export", EXPORT);
	BUILTIN("unset", UNSET);
	BUILTIN("env", ENV);
	BUILTIN("exit", EXIT);
	BUILTIN("set", SET);
	BUILTIN("type", TYPE);
	BUILTIN("jobs", JOBS);
	BUILTIN("fg", FG);
	BUILTIN("bg", BG);
	BUILTIN("hash", HASH);
	BUILTIN("test", TEST);
	BUILTIN("alias", ALIAS);
	BUILTIN("unalias", UNALIAS);
	BUILTIN("fc", FC);
	return BASIC;
}

void check_type(Command *cmd, Token token) {
	char *name = cmd->args->content;

	if (token == t_subshell)
		cmd->type = SUBSHELL;
	if (token == t_control_group)
		cmd->type = CONTROL_GROUP;
	if (!ft_strcmp(cmd->args->content, "["))
		cmd->type = CONDITION;
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

bool command_got_one_arg(Command *cmd) {
	Parser *args = cmd->args;
	bool redir = false;
	int n = 0;

	while (args) {
		if (IS_REDIR(args->token))
			redir = true;
		else if (!redir)
			n++;
		else
			redir = false;
		args = args->next;
	}

	return n == 1;
}

void analyze_command(Command *cmd) {
	if (cmd->type == SUBSHELL && !command_got_one_arg(cmd))
		cmd->error = unexpected_token;
	if (cmd->type == CONTROL_GROUP && !command_got_one_arg(cmd))
		cmd->error = unexpected_token;
}
