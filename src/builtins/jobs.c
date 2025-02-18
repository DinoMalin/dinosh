#include "builtins.h"

void jobs(Command *cmd, Context *ctx) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("jobs: too many args");

	for (Job *job=ctx->jobs; job; job=job->next) {
		ft_printf("[%d] %c %s ",
			job->index,
		 	job->is_current ? '+' : '-',
			job->state == RUNNING ? "Running" : "Stopped"
		);
		for (int i = 0; job->cmd->av[i]; i++) {
			ft_printf("%s", job->cmd->av[i]);
			if (job->cmd->av[i+1])
				ft_printf(" ");
		}
		ft_printf("\n");
	}
}
