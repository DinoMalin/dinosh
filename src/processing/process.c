#include "minishell.h"

Command *init_cmd() {
	Command *res = ft_calloc(1, sizeof(Command));

	res->av = ft_calloc(1, sizeof(char *));
	res->redirs = ft_calloc(1, sizeof(t_redir));
	return res;
}

void init_ac(Command *head) {
	while (head) {
		while (head->av[head->ac]) {
			head->ac++;
		}
		head = head->next;
	}
}

void check_type(Command *cmd) {
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
}

void check_redir(Command *cmd, Node **data) {
	Token type = (*data)->token;
	(*data) = (*data)->next;

	if (!(*data)) {
		cmd->error.type = empty_redir;
		return;
	}
	if (is_redir((*data)->token)) {
		cmd->error.type = redir_toward_redir;
		return;
	}

	Redir r_type = 0;
	if (type == from)
		r_type = r_from;
	else if (type == heredoc)
		r_type = r_heredoc;
	else if (type == to)
		r_type = r_to;
	else if (type == append)
		r_type = r_append;

	t_redir redirection = (t_redir){ft_strdup((*data)->content), r_type};
	cmd->redirs = clean_redirjoin(cmd->redirs, redirection);
}

void analyze_command(Command *cmd, Node **data, int *arg_index) {
	if (is_redir((*data)->token)) {
		check_redir(cmd, data);
		return;
	}

	if (*arg_index == 0) {
		cmd->cmd = ft_strdup((*data)->content);
		check_type(cmd);
	}
	cmd->av = clean_strsjoin(cmd->av, ft_strdup((*data)->content));
	(*arg_index)++;
}

Command *process(Node *data) {
	int data_index = 0;
	int arg_index = 0; // count the args w/o redirs
	Command *head = NULL;
	Command *curr = NULL;
	Command *new = NULL;

	while (data) {
		if (data->token == t_pipe) {
			arg_index = 0;
			data_index = 0;
		} else if (data_index == 0) {
			new = init_cmd();
			if (!head)
				head = new;
			else
				curr->next = new;
			curr = new;
			analyze_command(curr, &data, &arg_index);
			data_index++;
		} else {
			analyze_command(curr, &data, &arg_index);
			data_index++;
		}

		if (data)
			data = data->next;
	}
	
	init_ac(head);
	return head;
}
