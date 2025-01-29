#include "expand.h"

int max_index(Parser *head) {
	int max = 0;
	while (head) {
		if (head->index > max)
			max = head->index;
		head = head->next;
	}
	return max;
}

void merge(Parser *head) {
	Parser *next;
	Parser *after_next;

	while (head && head->next) {
		next = head->next;
		while (next && head->index == next->index) {
			head->content = clean_join(head->content, next->content);
			after_next = next->next;
			head->next = after_next;
			free_node(next);
			next = after_next;
		}
		head = head->next;
	}
}

void init_av(Command *cmd) {
	Parser *curr = cmd->args;

	while (curr) {
		Parser *next = curr->next;
		cmd->av = clean_strsjoin(cmd->av, ft_strdup(curr->content));
		curr = next;
		cmd->ac++;
	}
}

void expand(Command *cmd, Env *env) {
	Parser *curr = cmd->args;

	while (curr) {
		if (CAN_EXPAND(curr)) {
			char *expanded = expand_vars(curr->content, env);
			free(curr->content);
			curr->content = expanded;
		}
		if (CAN_WILDCARD(curr)) {
			curr = expand_wildcard(curr, max_index(cmd->args));
		}

		for (int i = 0; curr->content[i]; i++) {
			if (IS_STAR(curr->content[i]))
				curr->content[i] = '*';
		}

		curr = curr->next;
	}

	if (cmd->args)
		merge(cmd->args);

	init_av(cmd);
}
