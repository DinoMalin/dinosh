#include "minishell.h"

int g_signal = 0;

void sig_handler(int sig) {
	if (sig == SIGINT) {
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		g_signal = 130;
	}
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
		if (g_signal == 130) {
			ctx.code = 130;
			g_signal = 0;
		}
		UPDATE_CODE_VAR(ctx.code);

		if (!ctx.input)
			break;
		add_history(ctx.input);
		handle_input(&ctx);

		free(ctx.input);
	} while (!ctx.exit);

	free_av(ctx.env);
}
