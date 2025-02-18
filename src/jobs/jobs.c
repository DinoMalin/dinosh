#include "minishell.h"

void print_job(Job *job, int code) {
	char *args = ft_strdup("");

	for (int i = 0; job->cmd->av[i]; i++) {
		args = clean_join(args, job->cmd->av[i]);
		if (job->cmd->av[i+1])
			args = clean_join(args, " ");
	}

	if (!code) {
		printf("[%d] %c %s %s\n",
			job->index,
			job->is_current ? '+' : '-',
		 	(
				job->state == RUNNING ?	"Running" :
				job->state == STOPPED ?	"Stopped" :
		 								"Done"
		 	),
			args
		);
	} else {
		printf("[%d] %c %s(%d) %s\n",
			job->index,
			job->is_current ? '+' : '-',
		 	(
				job->state == RUNNING ?	"Running" :
				job->state == STOPPED ?	"Stopped" :
		 								"Done"
		 	),
		 	code,
			args
		);
	}
	free(args);
}

void print_pid(Job *job) {
	printf("[%d] %d\n", job->index, job->cmd->pid);
}

void add_job(Context *ctx, Command *cmd) {
	Job *new = malloc(sizeof(Job));

	int index = 1;
	for (Job *job=ctx->jobs; job; job=job->next) {
		if (job->index != index) {
			index = job->index;
			break;
		}
		index++;
	}

	for (Job *job=ctx->jobs; job; job=job->next) {
		if (job->is_current)
			job->is_current = false;
	}

	new->index = index;
	new->is_current = true;
	new->state = RUNNING;
	new->cmd = cmd;
	new->next = NULL;

	if (!ctx->jobs)
		ctx->jobs = new;
	else {
		Job *job = ctx->jobs;
		while (job->next && job->index != new->index - 1) {
			job = job->next;
		}
		Job *next = job->next;
		job->next = new;
		new->next = next;
	}
	print_pid(new);
}

void update_jobs(Context *ctx) {
	int status = 0;
	for (Job *job=ctx->jobs; job; job=job->next) {
		if (waitpid(job->cmd->pid, &status, WNOHANG)) {
			job->state = DONE;
			print_job(job, status);
		}
	}
}
