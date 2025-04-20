#include "minishell.h"

void expand_condition(Command *cmd) {
	Parser *curr = cmd->args;
	while (curr && curr->next) {
		curr = curr->next;
	}
	if (curr && ft_strcmp(curr->content, "]")) {
		cmd->error = unclosed_token;
		return;
	}

	curr = cmd->args;
	while (curr && curr->next && curr->next->next) {
		curr = curr->next;
	}
	if (curr && curr->next) {
		free_node(curr->next);
		curr->next = NULL;
	}

	cmd->type = TEST;
}
