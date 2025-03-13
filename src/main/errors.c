#include "minishell.h"

#define HANDLE_ERROR(err, msg)		\
	{									\
		if (head->error == err) {		\
			ERROR(msg);					\
			return true;				\
		}								\
	}

char get_token(Parser *data) {
	if (data->quoting == doubles)
		return '"';
	if (data->quoting == singles)
		return '\'';
	if (data->escaped)
		return '\\';
	if (data->token == t_control_group)
		return '{';
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
			ERROR("unclosed token: %c", get_token(head));
			return true;
		}
		head = head->next;
	}
	return false;
}

bool parse_error(Command *head) {
	while (head) {
		HANDLE_ERROR(empty_redir, "empty redirection");
		HANDLE_ERROR(unexpected_token, "unexpected token");
		HANDLE_ERROR(start_pipe, "start pipe");
		HANDLE_ERROR(unknown_token, "unknown token");
		HANDLE_ERROR(empty_subshell, "empty subshell");
		HANDLE_ERROR(empty_subshell, "empty subshell");
		HANDLE_ERROR(empty_control_group, "empty control group");
		HANDLE_ERROR(missing_parameter, "missing_parameter");
		head = head->next;
	}
	return false;
}

bool command_error(Command *head) {
	if (head->error == special) {
		ERROR("%s", head->error_message);
		return true;
	}

	HANDLE_ERROR(eheredoc, "failed heredoc");
	HANDLE_ERROR(ambiguous_redirect, "ambiguous redirect");
	HANDLE_ERROR(bad_substitution, "bad substitution");
	return false;
}
