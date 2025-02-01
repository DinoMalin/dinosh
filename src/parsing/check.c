#include "parsing.h"

void check_type(Command *cmd, Token token) {
	char *name = cmd->args->content;

	if (!ft_strcmp("echo", name))
		cmd->type = ECHO;
	else if (!ft_strcmp("cd", name))
		cmd->type = CD;
	else if (!ft_strcmp("pwd", name))
		cmd->type = PWD;
	else if (!ft_strcmp("export", name))
		cmd->type = EXPORT;
	else if (!ft_strcmp("unset", name))
		cmd->type = UNSET;
	else if (!ft_strcmp("env", name))
		cmd->type = ENV;
	else if (!ft_strcmp("exit", name))
		cmd->type = EXIT;
	else if (token == t_subshell)
		cmd->type = SUBSHELL;
	else if (ft_strchr(name, '=') && token == t_word)
		cmd->type = VAR;
}

void check_redir(Command *cmd, Parser **data) {
	Token type = (*data)->token;
	(*data) = (*data)->next;

	if (!(*data)) {
		cmd->error = empty_redir;
		return;
	}
	if (!CAN_REDIR((*data)->token)) {
		cmd->error = unexpected_token;
		return;
	}

	Redir r_type = 0;
	if (type == t_from)
		r_type = r_from;
	else if (type == t_heredoc)
		r_type = r_heredoc;
	else if (type == t_to)
		r_type = r_to;
	else if (type == t_append)
		r_type = r_append;

	merge_one_node(*data);
	t_redir redirection = (t_redir){ft_strdup((*data)->content), r_type};
	cmd->redirs = clean_redirjoin(cmd->redirs, redirection);

}
