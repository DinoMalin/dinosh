#include "execute.h"

void exit_fork(Command *head, Context *ctx) {
	if (head->type != CONTROL_SUBSTITUTION)
		free_cmds(head);
	free(ctx->access);
	free_env(ctx->env);
	free_jobs(ctx->jobs);
	free_garbage(ctx);
	free_hash(ctx);
	exit(ctx->code);
}

void fork_routine(Command *head, Command *cmd, Context *ctx, Pipes *pipes) {
	signal(SIGQUIT, sig_handler);
	redirect_pipe(cmd, pipes);
	redirect(cmd);

	if (cmd->error) {
		ctx->code = 1;
		exit_fork(head, ctx);
	}

	if (IS_BUILTIN(cmd->type))
		builtin(cmd, ctx);
	else if (  cmd->type == SUBSHELL
			|| cmd->type == CONTROL_SUBSTITUTION
			|| cmd->type == PROCESS_SUBSTITUTION_TO
			|| cmd->type == PROCESS_SUBSTITUTION_FROM) {
		char *interactivity = ((ctx->interactive
					&& cmd->to != BACKGROUND
					&& cmd->type != PROCESS_SUBSTITUTION_TO
					&& cmd->type != PROCESS_SUBSTITUTION_FROM
					)) ? "" : "--non-interactive";
		

		char *av[] = {ctx->access, interactivity, "-c", cmd->av[0], NULL};
		char **envp = get_envp(ctx->env);
		execve(ctx->access, av, envp);
		free_av(envp);
	} else if (cmd->type == BASIC) {
		if (cmd->av[0]) {
			char *path = find_path(ctx, cmd->av[0]);

			if (!path) {
				ERROR("%s: command not found", cmd->av[0]);
				ctx->code = 127;
			} else {
				char **envp = get_envp(ctx->env);
				execve(path, cmd->av, get_envp(ctx->env));
				free_av(envp);
				ctx->code = 126;
			}
		}
	}

	exit_fork(head, ctx);
}

void create_fork(Command *head, Command *cmd, Context *ctx, Pipes *pipes) {
	pid_t pid = fork();

	if (pid == -1)
		perror("dinosh: fork");
	else if (IS_CHILD(pid)) {
		setpgid(0, 0);
		signal(SIGTSTP, SIG_DFL);
		fork_routine(head, cmd, ctx, pipes);
	} else {
		setpgid(pid, pid);
		cmd->pid = pid;
	}
}

void wait_everything(Command *head, Command *until, Context *ctx) {
	int status = 0;

	while (head) {
		if (head->to == BACKGROUND) {
			head = head->next;
			continue;
		}

		if (head->pid == -1)
			break;
		if (head->pid) {
			signal(SIGTTOU, SIG_IGN);
			signal(SIGTTIN, SIG_IGN);

			if (ctx->interactive)
				PRINTPGRP(0, head->pid, head->av[0]);
			if (waitpid(head->pid, &status, WUNTRACED) == -1)
				perror("dinosh: wait");
			if (ctx->interactive)
				PRINTPGRP(0, ctx->gpid, head->av[0]);

			signal(SIGTTOU, SIG_DFL);
			signal(SIGTTIN, SIG_DFL);

			if (WIFEXITED(status)) {
				ctx->code = WEXITSTATUS(status);
			} else if (WIFSIGNALED(status)) {
				if (status == SIGQUIT_STATUS)
					dprintf(2, "Quit%s\n", WCOREDUMP(status) ? " (core dumped)" : "");
			} else if (WIFSTOPPED(status)) {
				add_job(ctx, head, STOPPED);
				head->to = BACKGROUND;
				ctx->code = 148;
			}
			head->pid = 0;
		} else {
			if (head->error)
				ctx->code = 1;
			else
				ctx->code = head->exit_code;
		}
		if (head == until)
			break;
		head = head->next;
	}
}

void execute(Command *head, Context *ctx) {
	Command *curr = head;
	Command *wait = head;
	Pipes pipes = {
		.curr = (int[]){-1, -1},
		.prev = (int[]){-1, -1}
	};

	fill_heredoc(head);
	while (curr) {
		DO_PIPE();
		if (!init_command(ctx, curr)) {
			curr = curr->next;
			continue;
		}

		if (IS_BUILTIN(curr->type) && !IS_PIPED(curr) && curr->to != BACKGROUND) {
			curr->pid = 0;
			fd_storage(STORE);
			redirect(curr);
			if (!curr->error)
				builtin(curr, ctx);
			fd_storage(RESTORE);
		} else if (curr->type == CONTROL_GROUP) {
			Context subctx = {
				.input = curr->av[0],
				.env = ctx->env,
				.exit = false,
				.gpid = ctx->gpid
			};
			handle_input(&subctx);
		} else {
			create_fork(head, curr, ctx, &pipes);

			xclose(pipes.prev[0]);
			xclose(pipes.prev[1]);

			pipes.prev[0] = pipes.curr[0];
			pipes.prev[1] = pipes.curr[1];
		}
		CHECK_TRANSMISSION();
		UPDATE_VAR_DURABILITY(ctx->env);
		update_code_var(ctx);

		curr = curr->next;
	}

	xclose(pipes.prev[0]);
	xclose(pipes.prev[1]);
	wait_everything(wait, NULL, ctx);
	update_code_var(ctx);
}
