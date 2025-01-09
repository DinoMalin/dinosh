#include "minishell.h"

char get_token(Node *data) {
	if (data->token == t_double_quotes)
		return '"';
	return '\'';
}

bool parsing_errors(Node *head) {
	while (head) {
		if (head->error)
			return true;
		head = head->next;
	}
	return false;
}
