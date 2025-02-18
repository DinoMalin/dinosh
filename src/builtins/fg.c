#include "builtins.h"

void fg(Command *cmd, Context *ctx) {
	if (cmd->ac > 1) {
		BUILTIN_ERROR("fg: didn't handle more args yet im lazy");
	}

	Job *job = ctx->jobs;
	while (job) {
		Job *next = job->next;
		if (job->is_current) {
			print_job(job, 0);
			waitpid(job->cmd->pid, &cmd->exit_code, 0);
			delete_job(ctx, job->index);
		}
		job = next;
	}
}
