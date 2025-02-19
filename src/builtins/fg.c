#include "builtins.h"

void fg(Command *cmd, Context *ctx) {
	if (cmd->ac > 2)
		BUILTIN_ERROR("fg: too many args");

	bool arg_mode = false;
	int arg = 0;
	if (cmd->ac == 2) {
		arg_mode = true;
		if (!is_number(cmd->av[1]))
			BUILTIN_ERROR("fg: numeric argument required");
		arg = ft_atoi(cmd->av[1]);
	}

	Job *job = ctx->jobs;
	while (job) {
		Job *next = job->next;
		if ((arg_mode && arg == job->index) || (!arg_mode && job->is_current)) {
			signal(SIGTTOU, SIG_IGN);
			if (tcsetpgrp(0, job->pid) == -1)
				BUILTIN_PERROR("dinosh: failed to tcsetpgrp");

			if (job->state != STOPPED)
				print_job(job, 0);
			else {
				if (kill(job->pid, SIGCONT) < 0)
					BUILTIN_PERROR("fg: failed to continue job");
				job->state = CONTINUED;
				print_job(job, 0);
				job->state = RUNNING;
			}

			int status = 0;

			while (waitpid(job->pid, &status, WUNTRACED) == -1 && errno == EINTR);
			if (tcsetpgrp(0, ctx->gpid) == -1)
				BUILTIN_PERROR("dinosh: failed to tcsetpgrp");
			signal(SIGTTOU, SIG_DFL);

			if (WIFEXITED(status)) {
				cmd->exit_code = WEXITSTATUS(status);
				delete_job(ctx, job->index);
			} else if (WIFSTOPPED(status)) {
				cmd->exit_code = 148;
				job->state = STOPPED;
				print_job(job, 0);
			}
		}
		job = next;
	}
}
