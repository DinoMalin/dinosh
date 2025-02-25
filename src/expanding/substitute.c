#include "expand.h"

char *substitute(Context *ctx, Parser *el) {
	Pipes pipes = {
		.curr = {-1, -1},
		.prev = {-1, -1}
	};
	if (pipe(pipes.curr) < 0) {
		perror("dinosh: pipe");
		return NULL;
	}

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
		return NULL;
	}

	if (pid == -1)
		return NULL;

	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	SETPGRP(0, pid);
	SETPGRP(1, pid);
	int status = 0;
	if (waitpid(pid, &status, WUNTRACED) == -1) {
		perror("dinosh: wait");
		return NULL;
	}
	SETPGRP(0, ctx->gpid);
	SETPGRP(1, ctx->gpid);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);

	if (status != 0)
		return NULL;

	char buff[200] = {};
	close(pipes.curr[1]);

	char *res = ft_strdup("");
	while (read(pipes.curr[0], buff, 199) > 0) {
		res = clean_join(res, buff);
	}

	if (res[ft_strlen(res-1)] == '\n') {
		char *tmp = res;
		res = ft_substr(res, 0, ft_strlen(res)-1); // remove the last \n
		free(tmp);
	}
	return res;
}

void control_substitution(Context *ctx, Command *cmd, Parser *el, int max) {
	char *res = substitute(ctx, el);
	if (res) {
		free(el->content);
		el->content = ft_strdup("");
		add_tokenized_args(el, res, max);
	} else
		cmd->error = bad_substitution;
}
