#include "execute.h"

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
	if (IS_CHILD(pid))
		fork_routine(&cmd, &cmd, ctx, &pipes);
	else if (pid == -1) {
		perror("fork");
		return NULL;
	}

	if (pid == -1)
		return NULL;

	int status = 0;
	if (waitpid(pid, &status, WUNTRACED) == -1) {
		perror("dinosh: wait");
		return NULL;
	}

	if (status != 0)
		return NULL;
	char buff[200] = {};
	int nbytes = 0;
	close(pipes.curr[1]);
	char *res = ft_strdup("");
	while ((nbytes=read(pipes.curr[0], buff, 199)) > 0) {
		res = clean_join(res, buff);
	}

	char *tmp = res;
	res = ft_substr(res, 0, ft_strlen(res)-1); // remove the last \n
	free(tmp);
	return res;
}

void control_substitution(Context *ctx, Command *cmd) {
	Parser *curr = cmd->args;
	while (curr) {
		if (curr->token == t_control_substitution) {
			char *res = substitute(ctx, curr);
			if (res) {
				free(curr->content);
				curr->content = res;
			} else {
				cmd->error = bad_substitution;
				return;
			}	
		}
		curr = curr->next;
	}
}
