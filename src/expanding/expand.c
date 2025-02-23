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
		bool can_expand = false;
		Parser *node = curr;
		while (node && node->id == curr->id) {
			if (CAN_EXPAND(node)
				|| node->token == t_wildcard
				|| node->token == t_arithmetic
				|| node->token == t_tilde
			) {
				can_expand = true;
			}
			node = node->next;
		}
		if (!can_expand) {
			merge_one_node(curr);
			curr->expand_id = -1;
		}
		curr = node;
	}

	curr = cmd->args;
	while (curr) {
		if (curr->token == t_var)
			expand_one_var(env, curr, max_id(cmd->args));
		else if (CAN_EXPAND(curr))
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

	curr = cmd->args;
	while (curr) {
		if (curr->token == t_arithmetic)
			arithmetic(env, curr);
		if (curr->token == t_tilde)
			tilde(curr);
		curr = curr->next;
	}
}
