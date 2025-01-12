#include "minishell.h"

char get_token(Node *data) {
	if (data->token == t_double_quotes)
		return '"';
	if (data->token == t_single_quotes)
		return '\'';
	return '(';
}

bool has_parsing_errors(Node *head) {
	while (head) {
		if (head->error)
			return true;
		head = head->next;
	}
	return false;
}

bool parsing_error(Node *head) {
	while (head) {
		if (head->error) {
			dprintf(2, "dinosh: unclosed token: %c\n", get_token(head));
			return true;
		}
		head = head->next;
	}
	return false;
}

bool process_error(Command *head) {
	while (head) {
		if (head->error == empty_redir) {
			dprintf(2, "dinosh: empty redirection\n");
			return true;
		}
		if (head->error == redir_toward_redir) {
			dprintf(2, "dinosh: redirection toward redirection\n");
			return true;
		}
		if (head->error == start_pipe) {
			dprintf(2, "dinosh: command cannot start with a pipe\n");
			return true;
		}
		head = head->next;
	}
	return false;
}
