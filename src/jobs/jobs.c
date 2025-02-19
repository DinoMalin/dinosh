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
				job->state == DONE	  ?	"Done"	  :
										"Continued"
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
				job->state == DONE	  ?	"Done"	  :
										"Continued"
		 	),
		 	code,
			args
		);
	}
	free(args);
}

void print_pid(Job *job) {
	printf("[%d] %d\n", job->index, job->pid);
}

void add_job(Context *ctx, Command *cmd, State state) {
	Job *new = ft_calloc(1, sizeof(Job));

	int index = 1;
	for (Job *job=ctx->jobs; job; job=job->next) {
		if (job->index != index)
			break;
		index++;
	}

	for (Job *job=ctx->jobs; job; job=job->next) {
		if (job->is_current)
			job->is_current = false;
	}

	new->index = index;
	new->is_current = true;
	new->state = state;
	new->pid = cmd->pid;
	new->cmd = cmd;

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

	if (state == RUNNING)
		print_pid(new);
	else
		print_job(new, 0);

}

void delete_job(Context *ctx, int id) {
	Job *job = ctx->jobs;
	Job *prec = ctx->jobs;

	while (job) {
		Job *next = job->next;
		if (job->index == id) {
			if (job == ctx->jobs) {
				ctx->jobs = next;
				free_job(job);
			} else {
				prec->next = next;
				free_job(job);
			}
		}
		prec = job;
		job = next;
	}
}

void update_jobs(Context *ctx) {
	int status = 0;

	Job *prec = ctx->jobs;
	Job *job = ctx->jobs;
	while (job) {
		Job *next = job->next;
		if (waitpid(job->pid, &status, WNOHANG)) {
			job->state = DONE;
			print_job(job, status);
			
			if (job == ctx->jobs) {
				ctx->jobs = next;
				free_job(job);
			} else {
				prec->next = next;
				free_job(job);
			}
		}
		prec = job;
		job = next;
	}
}

bool is_job(Context *ctx, Command *cmd) {
	for (Job *job=ctx->jobs; job; job=job->next) {
		if (job->cmd == cmd)
			return true;
	}
	return false;
}
