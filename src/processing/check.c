#include "process.h"

void init_ac(Command *head) {
	while (head) {
		while (head->av[head->ac]) {
			head->ac++;
		}
		head = head->next;
	}
}

void check_type(Command *cmd, Token token) {
	if (!ft_strcmp("echo", cmd->cmd))
		cmd->type = ECHO;
	else if (!ft_strcmp("cd", cmd->cmd))
		cmd->type = CD;
	else if (!ft_strcmp("pwd", cmd->cmd))
		cmd->type = PWD;
	else if (!ft_strcmp("export", cmd->cmd))
		cmd->type = EXPORT;
	else if (!ft_strcmp("unset", cmd->cmd))
		cmd->type = UNSET;
	else if (!ft_strcmp("env", cmd->cmd))
		cmd->type = ENV;
	else if (!ft_strcmp("exit", cmd->cmd))
		cmd->type = EXIT;
	else if (token == t_subshell)
		cmd->type = SUBSHELL;
}

void check_redir(Command *cmd, Parser **data) {
	Token type = (*data)->token;
	(*data) = (*data)->next;

	if (!(*data)) {
		cmd->error = empty_redir;
		return;
	}
	if (is_redir((*data)->token)) {
		cmd->error = redir_toward_redir;
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

	t_redir redirection = (t_redir){ft_strdup((*data)->content), r_type};
	cmd->redirs = clean_redirjoin(cmd->redirs, redirection);
}
