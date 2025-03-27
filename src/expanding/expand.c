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

void expand(Context *ctx, Command *cmd) {
	Parser *curr = cmd->args;
	while (curr) {
		if (curr->token == t_wordvar)
			var(ctx->env, curr, max_id(cmd->args));
		if (curr->token == t_var)
			parameter(ctx->env, cmd, curr, max_id(cmd->args));
		if (curr->token == t_arithmetic)
			arithmetic(ctx->env, curr);
		if (curr->token == t_tilde)
			tilde(curr);
		if (curr->token == t_control_substitution)
			control_substitution(ctx, cmd, curr, max_id(cmd->args));
		if (curr->token == t_process_substitution_to
			|| curr->token == t_process_substitution_from)
			process_substitution(ctx, cmd, curr);
		curr = curr->next;
	}

	curr = cmd->args;
	while (curr) {
		if (this_id_has_wildcard(curr))
			curr = expand_wildcard(curr, max_id(cmd->args));

		int id = curr->id;
		while (curr && curr->id == id)
			curr = curr->next;
	}
}
