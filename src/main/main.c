#include "minishell.h"

extern int g_signal;

void config(int ac, char **av, Context *ctx) {
	bool default_config = true;

	for (int i = 0; i < ac; i++) {
		if (!ft_strcmp(av[i], "--rcfile") && i < ac - 1) {
			default_config = !read_file(av[i + 1], ctx);
		}
	}

	if (default_config) {
		modify_env(&ctx->env, "PROMPT", "dinosh> ", INTERN);
	}
}

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;

	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);

	Context ctx = {
		.input = NULL,
		.env = create_env(envp),
		.exit = false,
	};
	init_basic_vars(&ctx);

	config(ac, av, &ctx);
	rl_event_hook = &rl_hook;

	do {
		char *prompt = ft_getenv(ctx.env, "PROMPT");
		if (!prompt)
			prompt = "";
		ctx.input = readline(prompt);
		if (g_signal == (int)0xDEADBEEF) {
			ctx.code = 130;
			g_signal = 0;
		}

		if (!ctx.input)
			break;
		add_history(ctx.input);
		handle_input(&ctx);

		free(ctx.input);
	} while (!ctx.exit);

	free_env(ctx.env);
	return ctx.code;
}
