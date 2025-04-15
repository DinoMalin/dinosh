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
		data->id += max + 1;
		data = data->next;
	}
}

int search_alias(Parser *data, Alias *alias) {
	bool new_command = true;

	while (data) {
		if (new_command) {
			new_command = false;
			if ((!data->next || (data->id != data->next->id))
			&& data->quoting == none && !data->escaped) {
				if (is_alias(data->content, alias))
					return data->id;
			}
		}
		if (!new_command && (data->token == t_and || data->token == t_or
		|| data->token == t_pipe || data->token == t_semicolon
		|| data->token == t_bg)) {
			new_command = true;
		}
		data = data->next;
	}
	return -1;
}

Parser *expand_alias(Parser *data, Alias *alias) {
	Parser *head = data;

	int alias_id = search_alias(data, alias);
	if (alias_id == -1)
		return data;

	while (data->id != alias_id)
		data = data->next;

	char *alias_content = is_alias(data->content, alias);
	Parser *alias_tokens = tokenize(alias_content);

	int max = max_id(head);
	add_max_id(alias_tokens, max);

	Parser *last_alias_token = alias_tokens;
	while (last_alias_token->next)
		last_alias_token = last_alias_token->next;

	last_alias_token->next = data->next;

	if (head == data) {
		free_node(data);
		head = alias_tokens;
	} else {
		Parser *prev = head;
		while (prev && prev->next != data)
			prev = prev->next;

		free_node(data);
		prev->next = alias_tokens;
	}

	return expand_alias(head, alias);
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
