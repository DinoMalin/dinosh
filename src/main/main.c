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

	tests_parsing(envp);
	Context ctx = {
		.input = NULL,
		.env = copy_env(envp),
		.exit = false,
	};

	do {
		ctx.input = readline("dinosh> ");
		if (ctx.input)
			handle_input(&ctx);
		else
			ctx.exit = true;
		free(ctx.input);
	} while (!ctx.exit);

	free_av(ctx.env);
}
