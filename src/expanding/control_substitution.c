#include "expand.h"
#define WAIT(s)											\
	{													\
		signal(SIGTTOU, SIG_IGN);						\
		signal(SIGTTIN, SIG_IGN);						\
		if (ctx->interactive)							\
			SETPGRP(0, pid);							\
		if (waitpid(pid, &s, WUNTRACED) == -1) {		\
			perror("dinosh: wait");						\
			return NULL;								\
		}												\
		if (ctx->interactive)							\
			SETPGRP(0, ctx->gpid);						\
		signal(SIGTTOU, SIG_DFL);						\
		signal(SIGTTIN, SIG_DFL);						\
	}

#define TRIM_NEWLINE()										\
	{														\
		int len = ft_strlen(res);							\
		if (len && res[len-1] == '\n') {					\
			char *tmp = res;								\
			res = ft_substr(res, 0, ft_strlen(res)-1);		\
			free(tmp);										\
		}													\
	}

bool fork_process(Context *ctx, Parser *el, int *pipe_fd) {
	Pipes pipes = {.prev = (int[]){-1, -1}};
	pipes.curr = pipe_fd;

	Command cmd = {
		.av = (char*[]){el->content, NULL},
		.type = CONTROL_SUBSTITUTION,
		.to = PIPE
	};

	int pid = fork();
	if (!pid)
		fork_routine(&cmd, &cmd, ctx, &pipes);
	else if (pid == -1) {
		perror("fork");
		return false;
	}

	int status = 0;
	WAIT(status);
	if (status != 0) {
		close(pipe_fd[1]);
		close(pipe_fd[0]);
		return false;
	}

	return true;
}

static char *substitute(Context *ctx, Parser *el) {
	int pipe_fd[2];
	if (pipe(pipe_fd) < 0) {
		perror("dinosh: pipe");
		return NULL;
	}

	if (!fork_process(ctx, el, pipe_fd))
		return NULL;

	char buff[BUFFER_SIZE] = {};
	close(pipe_fd[1]);
	char *res = ft_strdup("");
	int ret = 0;
	while ((ret = read(pipe_fd[0], buff, BUFFER_SIZE-1)) > 0) {
		buff[ret] = 0;
		res = clean_join(res, buff);
	}
	close(pipe_fd[0]);

	TRIM_NEWLINE();
	return res;
}

void control_substitution(Context *ctx, Command *cmd, Parser *el, int max) {
	char *res = substitute(ctx, el);
	if (res && el->quoting == doubles) {
		free(el->content);
		el->content = res;
	} else if (res) {
		add_tokenized_args(el, res, max);
		free(res);
	} else {
		cmd->error = bad_substitution;
	}
}
