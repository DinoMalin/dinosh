#include "minishell.h"

void handle_input(Context *ctx) {
	Parser *data = tokenize(ctx->input);
	if (token_error(data)) {
		free_list(data);
		return;
	}

	Command *cmd = parse(data);
	free_list(data);

	if (parse_error(cmd)) {
		free_cmds(cmd);
		return;
	}

	execute(cmd, ctx);
	free_cmds(cmd);
}
