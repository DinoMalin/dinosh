#include "minishell.h"
#include <readline/readline.h>

extern int g_signal;
Env *environ = NULL;
Hash **hash_table = NULL;
int *hash_len = NULL;

bool check_interactive(int ac, char **av) {
	for (int i = 0; i < ac; i++) {
		if (!ft_strcmp(av[i], "--non-interactive"))
			return false;
	}
	return true;
}

void config(int ac, char **av, Context *ctx) {
	bool default_config = true;

	for (int i = 0; i < ac; i++) {
		if (!ft_strcmp(av[i], "--rcfile") && i < ac - 1)
			default_config = !read_file(av[i + 1], ctx);
	}

	if (default_config) {
		modify_env(&ctx->env, "PROMPT", "dinosh> ", INTERN, -1);
	}
}

bool run_av(int ac, char **av, Context *ctx) {
	if (ac < 2)
		return false;

	int i = 0;
	for (; av[i]; i++) {
		if (!ft_strcmp("-c", av[i]))
			break;
	}
	if (!av[i])
		return false;
	if (!av[i+1]) {
		ERROR("-c: option requires an argument");
		ctx->code = 2;
		return true;
	}

	ctx->input = av[i+1];
	handle_input(ctx);
	return true;
}

bool run_script(int ac, char **av, Context *ctx) {
	for (int i = 1; i < ac; i++) {
		if (!ft_strcmp(av[i], "--rcfile")) {
			i++;
		} else if (ft_strcmp(av[i], "--non-interactive")) {
			read_file(av[i], ctx);
			return true;
		}
	}
	return false;
}

void run_prompt(Context *ctx) {
	rl_event_hook = &rl_hook;

	while (!ctx->exit) {
		if (!isatty(STDIN_FILENO))
			ctx->input = get_next_line(0);
		else {
			char *prompt = ft_getenv(ctx->env, "PROMPT");
			if (!prompt)
				prompt = "";
			ctx->input = readline(prompt);
		}
		if (g_signal == (int)0xDEADBEEF) {
			ctx->code = 130;
			g_signal = 0;
		}

		if (!ctx->input)
			break;
		if (ft_strlen(ctx->input)) {
			char *tmp = ctx->input;

			ctx->input = expand_exclamation_mark(ctx->input);
			if (!ctx->input) {
				Command err = {.error = event_not_found};
				parse_error(&err);
			} else {
				add_history(ctx->input);
				append_history(1, HISTORY_FILE);
				handle_input(ctx);
			}

			free(tmp);
		}

		free(ctx->input);
		update_jobs(ctx);
	}
}

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;
	rl_attempted_completion_function = completion;
	rl_outstream = stderr;
	rl_bind_key_in_map('v', open_vim, vi_movement_keymap);

	signal(SIGINT, sig_handler);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN); //maybe more signals

	environ = create_env(envp);
	Context ctx = {
		.input = NULL,
		.env = environ,
		.exit = false,
		.gpid = getpgrp(),
		.access = ft_strdup(av[0]),
		.interactive = check_interactive(ac, av),
		.garbage = NULL
	};
	hash_table = &(ctx.hash);
	hash_len = &(ctx.hash_len);
	init_basic_vars(&ctx);
	milk_path(&ctx);

	if (!run_av(ac, av, &ctx) && !run_script(ac, av, &ctx)) {
		config(ac, av, &ctx);
		read_history(HISTORY_FILE);
		run_prompt(&ctx);
	}

	free_env(ctx.env);
	unalias_all(&ctx);
	free_jobs(ctx.jobs);
	free(ctx.access);
	free_hash(&ctx);
	rl_clear_history();
	return ctx.code;
}
