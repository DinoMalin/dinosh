#include "minishell.h"

Command *init_cmd() {
	Command *res = ft_calloc(1, sizeof(Command));

	res->av = ft_calloc(1, sizeof(char *));
	res->redirs = ft_calloc(1, sizeof(t_redir));
	return res;
}

void analyze_command(Command *cmd, Node **data, int *arg_index) {
	if (is_redir((*data)->token)) {
		check_redir(cmd, data);
		return;
	}

	if (*arg_index == 0) {
		cmd->cmd = ft_strdup((*data)->content);
		check_type(cmd, (*data)->token);
	}
	cmd->av = clean_strsjoin(cmd->av, ft_strdup((*data)->content));
	(*arg_index)++;
}

Command *process(Node *data) {
	int data_index = 0;
	int arg_index = 0; // count the args w/o redirs
	Command *head = NULL;
	Command *curr = NULL;

	while (data) {
		if (data->token == t_pipe) {
			arg_index = 0;
			data_index = 0;
		} else if (data_index == 0) {
			Command *new = init_cmd();
			ADD_COMMAND(head, new, curr);
			curr = new;
		}
		if (data->token != t_pipe) {
			analyze_command(curr, &data, &arg_index);
			data_index++;
		}
		if (data)
			data = data->next;
	}
	
	init_ac(head);
	return head;
}
