#include "builtins.h"

void bg(Command *cmd, Context *ctx) {
	if (cmd->ac > 2)
		BUILTIN_ERROR("bg: too many args");

	bool arg_mode = false;
	int arg = 0;
	if (cmd->ac == 2) {
		arg_mode = true;
		if (!is_number(cmd->av[1]))
			BUILTIN_ERROR("bg: numeric argument required");
		arg = ft_atoi(cmd->av[1]);
	}

	Job *job = ctx->jobs;
	while (job) {
		if ((arg_mode && arg == job->index) || (!arg_mode && job->is_current)) {
			if (job->state == STOPPED) {
				if (kill(job->pid, SIGCONT) < 0)
					BUILTIN_PERROR("bg: failed to continue job");
				job->state = CONTINUED;
				print_job(job, 0);
				job->state = RUNNING;
		 	} else {
				BUILTIN_ERROR("bg: job already in background");
			}
		}
		job = job->next;
	}
}
