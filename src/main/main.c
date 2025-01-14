#include "minishell.h"

extern int g_signal;

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;
	(void)envp;
	tests_parsing(envp);

	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);

	Context ctx = {
		.input = NULL,
		.env = copy_env(envp),
		.exit = false,
	};
	rl_event_hook = &rl_hook;

	do {
		ctx.input = readline("dinosh> ");
		if (g_signal == 130) {
			ctx.code = 130;
			g_signal = 0;
			skip_hook(true, false);
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
