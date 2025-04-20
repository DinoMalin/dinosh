#include "parsing.h"

void add_arg(Command *cmd, Parser *new) {
	Parser *cpy = ft_calloc(1, sizeof(Parser));
	cpy->content = ft_strdup(new->content);
	cpy->token = new->token;
	cpy->id = new->id;
	cpy->quoting = new->quoting;
	cpy->escaped = new->escaped;

	if (cmd->args) {
		Parser *last = cmd->args;
		while (last->next) {
			last = last->next;
		}
		last->next = cpy;
	} else {
		cmd->args = cpy;
	}
}

Command *init_cmd(Transmission from) {
	Command *res = ft_calloc(1, sizeof(Command));

	res->av = ft_calloc(1, sizeof(char *));
	res->redirs = ft_calloc(1, sizeof(t_redir));
	res->from = from;
	res->pid = -1;
	return res;
}

// fill the command properties, detect the errors
void analyze_arg(Command *cmd, Parser **data, int *arg_index) {
	if ((*data)->token == t_unknown)
		cmd->error = unknown_token;
	if ((*data)->token == t_missing_parameter)
		cmd->error = missing_parameter;
	if ((*data)->token == t_unexpected)
		cmd->error = unexpected_token;

	if (IS_REDIR((*data)->token))
		check_redir_errors(cmd, data);

	add_arg(cmd, *data);
	if (*arg_index == 0)
		check_type(cmd, (*data)->token);

	(*arg_index)++;
}

Command *parse(Parser *data) {
	int data_index = 0;
	int arg_index = 0; // count the args w/o redirs
	Error error = no_error;
	Transmission from = ANY;

	Command *head = NULL;
	Command *curr = NULL;

	while (data) {
		if (data_index == 0 && !ft_strlen(data->content)) {
			if (data->token == t_subshell)
				error = empty_subshell;
			if (data->token == t_control_group)
				error = empty_control_group;
			data = data->next;
			continue;
		}

		PROCESS_TRANSMISSION(t_pipe, PIPE, true);
		PROCESS_TRANSMISSION(t_and, AND, true);
		PROCESS_TRANSMISSION(t_or, OR, true);
		PROCESS_TRANSMISSION(t_semicolon, SEMICOLON, false);
		PROCESS_TRANSMISSION(t_bg, BACKGROUND, false);

		if (data_index == 0) {
			Command *new = init_cmd(from);
			ADD_COMMAND(head, new, curr);
			curr = new;
		}

		analyze_arg(curr, &data, &arg_index);
		data_index++;

		if (data)
			data = data->next;
	}
	
	analyze_command(curr);
	TREAT_ERRORS(head);
	return head;
}
