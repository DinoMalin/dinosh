#include "expand.h"

bool this_id_has_wildcard(Parser *head) {
	Parser *curr = head;
	while (curr && curr->id == head->id) {
		if (curr->token == t_wildcard)
			return true;
		curr = curr->next;
	}
	
	return false;
}

int max_id(Parser *head) {
	int max = 0;
	while (head) {
		if (head->id > max)
			max = head->id;
		head = head->next;
	}
	return max;
}

void expand(Command *cmd, Env *env) {
	Parser *curr = cmd->args;
	while (curr) {
		if (!CAN_EXPAND(curr) && !this_id_has_wildcard(curr))
			merge_one_node(curr);
		curr = curr->next;
	}

	curr = cmd->args;
	while (curr) {
		if (CAN_EXPAND(curr))
			expand_vars(env, curr, max_id(cmd->args));
		curr = curr->next;
	}

	curr = cmd->args;
	while (curr) {
		if (this_id_has_wildcard(curr)) {
			curr = expand_wildcard(curr, max_id(cmd->args));
		}
		int id = curr->id;
		while (curr && curr->id == id)
			curr = curr->next;
	}
}
