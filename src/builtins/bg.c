#include "builtins.h"

void bg(Command *cmd, Context *ctx) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("bg: didn't handle more args yet im lazy");

	Job *job = ctx->jobs;
	while (job) {
		if (job->is_current) {
			if (job->state == STOPPED) {
				if (kill(job->cmd->pid, SIGCONT) < 0)
					BUILTIN_PERROR("bg: failed to continue job");
				job->state = CONTINUED;
				print_job(job, 0);
				job->state = RUNNING;
		 	}
		}
		job = job->next;
	}
}
