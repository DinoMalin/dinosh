#include "minishell.h"

void update_code_var(Context *ctx) {
	char *s = ft_itoa(ctx->code);
	modify_env(&ctx->env, "?", s);
	set_special(ctx->env, "?", true);
	set_intern(ctx->env, "?", true);
	free(s);
}

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
