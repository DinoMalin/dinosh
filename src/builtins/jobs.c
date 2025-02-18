#include "builtins.h"

void jobs(Command *cmd, Context *ctx) {
	if (cmd->ac > 1)
		BUILTIN_ERROR("jobs: too many args");

	for (Job *job=ctx->jobs; job; job=job->next) {
		print_job(job, 0);
	}
}
