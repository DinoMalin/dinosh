#include "minishell.h"

char get_token(Parser *data) {
	if (data->token == t_double_quotes)
		return '"';
	if (data->token == t_single_quotes)
		return '\'';
	return '(';
}

bool has_token_errors(Parser *head) {
	while (head) {
		if (head->error)
			return true;
		head = head->next;
	}
	return false;
}

bool token_error(Parser *head) {
	while (head) {
		if (head->error) {
			dprintf(2, "dinosh: unclosed token: %c\n", get_token(head));
			return true;
		}
		head = head->next;
	}
	return false;
}

bool parse_error(Command *head) {
	while (head) {
		if (head->error == empty_redir) {
			dprintf(2, "dinosh: empty redirection\n");
			return true;
		}
		if (head->error == unexpected_token) {
			dprintf(2, "dinosh: unexpected token\n");
			return true;
		}
		if (head->error == start_pipe) {
			dprintf(2, "dinosh: command cannot start with a pipe\n");
			return true;
		}
		if (head->error == unknown_token) {
			dprintf(2, "dinosh: unknown token\n");
			return true;
		}
		if (head->error == empty_subshell) {
			dprintf(2, "dinosh: empty subshell\n");
			return true;
		}
		if (head->error == ambiguous_redirect) {
			dprintf(2, "dinosh: ambiguous redirect\n");
			return true;
		}
		head = head->next;
	}
	return false;
}
