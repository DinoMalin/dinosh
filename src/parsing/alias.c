#include "minishell.h"

void add_max_id(Parser *data, int max) {
	while(data) {
		data->id += max + 1;
		data = data->next;
	}
}

Parser *search_alias(Parser *data, Alias *alias) {
	bool new_command = true;
	
	while (data) {
		if (new_command) {
			if (IS_REDIR(data->token) && data->next->next)
				data = data->next->next;
			new_command = false;
			if ((!data->next || (data->id != data->next->id))
			&& data->quoting == none && !data->escaped) {
				if (get_alias(data->content, alias))
					return data;
			}
		}
		if (!new_command && (data->token == t_and || data->token == t_or
		|| data->token == t_pipe || data->token == t_semicolon
		|| data->token == t_bg)) {
			new_command = true;
		}
		data = data->next;
	}
	return NULL;
}

Parser *expand_alias(Parser *data, Alias *alias) {
	Parser *head = data;

	Parser *tmp = search_alias(data, alias);
	if (!tmp)
		return data;
	else
		data = tmp;

	char *alias_content = get_alias(data->content, alias);
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
