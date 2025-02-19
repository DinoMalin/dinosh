#include "builtins.h"

void fg(Command *cmd, Context *ctx) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("fg: didn't handle more args yet im lazy");

	Job *job = ctx->jobs;
	while (job) {
		Job *next = job->next;
		if (job->is_current) {

			if (job->state != STOPPED)
				print_job(job, 0);
			else {
				if (kill(job->cmd->pid, SIGCONT) < 0)
					BUILTIN_PERROR("bg: failed to continue job");
				job->state = CONTINUED;
				print_job(job, 0);
				job->state = RUNNING;
			}

			int status = 0;
			waitpid(job->cmd->pid, &status, WUNTRACED);
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
