#include "minishell.h"

void sig_handler(int sig) {
	if (sig == SIGINT) {
		rl_replace_line("", 0);
		printf("\n");
		rl_on_new_line();
		rl_redisplay();
		// exit_code to 130
	}
}

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

	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);

	Context ctx = {
		.input = NULL,
		.env = copy_env(envp),
		.exit = false,
	};

	do {
		ctx.input = readline("dinosh> ");
		if (!ctx.input)
			break;
	
		add_history(ctx.input);
		handle_input(&ctx);
		free(ctx.input);
	} while (!ctx.exit);

	free_av(ctx.env);
}
