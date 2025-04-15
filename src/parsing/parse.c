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
void analyze_command(Command *cmd, Parser **data, int *arg_index) {
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

void add_max_id(Parser *data, int max) {
	while(data) {
		data->id += max;
		data = data->next;
	}
}

Parser *expand_alias(Parser *data, Alias *alias) {
	if ((data->next && data->id == data->next->id)
		|| data->quoting != none || data->escaped) {
		return data;
	}

	char *alias_content = is_alias(data->content, alias);
	if (!alias_content)
		return data;

	Parser *alias_data = tokenize(alias_content);
	Parser* data_cpy = alias_data;
	int max = max_id(data);
	add_max_id(alias_data, max);
	while(alias_data) {
		if (!alias_data->next) {
			alias_data->next = data->next;
			free_node(data);
			break;
		}
		alias_data = alias_data->next;
	}
	data_cpy = expand_alias(data_cpy, alias);
	return data_cpy;
}

Command *parse(Parser *data, Alias *alias) {
	int data_index = 0;
	int arg_index = 0; // count the args w/o redirs
	Error error = no_error;
	Transmission from = ANY;

	Command *head = NULL;
	Command *curr = NULL;

	data = expand_alias(data, alias);
	Parser *data_cpy = data;

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
		if (data->token != t_pipe) {
			analyze_command(curr, &data, &arg_index);
			data_index++;
		}
		if (data)
			data = data->next;
	}

	free_list(data_cpy);
	TREAT_ERRORS(head);
	return head;
}
