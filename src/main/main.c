#include "minishell.h"

void handle_input(Context *ctx) {
	Node *data = parse(ctx->input, ctx->env);
	if (parsing_error(data)) {
		free_list(data);
		return;
	}

	Command *cmd = process(data);
	free_list(data);

	if (process_error(cmd)) {
		free_cmds(cmd);
		return;
	}

	execute(cmd, ctx);
	free_cmds(cmd);
}

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;

	char **new_env = copy_env(envp);
	tests_parsing(envp);

	char *str = "echo test";

	Context ctx;
	ctx.input = str;
	ctx.env = new_env;

	handle_input(&ctx);
	free_av(ctx.env);
}
