#include "expand.h"

static bool fork_process(Context *ctx, Parser *el, int *pipe_fd, Token token) {
	Pipes pipes = {
		.prev = (int[]){-1, -1},
		.curr = (int[]){-1, -1}
	};

	if (token == t_process_substitution_to)
		pipes.prev = pipe_fd;
	else
		pipes.curr = pipe_fd;

	Command cmd = {
		.av = (char*[]){el->content, NULL},
		.type = token == t_process_substitution_to ?
						PROCESS_SUBSTITUTION_TO :
						PROCESS_SUBSTITUTION_FROM,
		.to = token == t_process_substitution_from ? PIPE : ANY,
		.from = token == t_process_substitution_to ? PIPE : ANY
	};

	int pid = fork();
	if (!pid)
		fork_routine(&cmd, &cmd, ctx, &pipes);
	else if (pid == -1) {
		perror("fork");
		return false;
	} else {
		add_garbage(ctx, -1, pid);
	}

	return true;
}

static char *substitute(Context *ctx, Parser *el) {
	int pipe_fd[2];
	if (pipe(pipe_fd) < 0) {
		perror("dinosh: pipe");
		return NULL;
	}

	if (!fork_process(ctx, el, pipe_fd, el->token))
		return NULL;

	char *fd;
	if (el->token == t_process_substitution_to) {
		fd = ft_itoa(pipe_fd[1]);
		close(pipe_fd[0]);
		add_garbage(ctx, pipe_fd[1], 0);
	} else {
		fd = ft_itoa(pipe_fd[0]);
		close(pipe_fd[1]);
		add_garbage(ctx, pipe_fd[0], 0);
	}

	char *res = ft_strjoin("/proc/self/fd/", fd);
	free(fd);
	return res;
}

void process_substitution(Context *ctx, Command *cmd, Parser *el) {
	char *res = substitute(ctx, el);
	if (!res) {
		cmd->error = bad_substitution;
		return;
	}

	free(el->content);
	el->content = res;
}
