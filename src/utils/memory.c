#include "minishell.h"

void free_env(Env *env) {
	while (env) {
		Env *next = env->next;
		free(env->var);
		free(env->value);
		free(env->old_value);
		free(env);
		env = next;
	}
}

void free_av(char **av) {
 	int i = 0;
	while (av[i]) {
		free(av[i]);
		i++;
	}
	free(av);
}

void free_redir(t_redir *redir) {
	int i = 0;
	while (redir[i].file) {
		free(redir[i].file);
		i++;
	}
	free(redir);
}

void free_cmd(Command *cmd) {
	free_av(cmd->av);
	free_redir(cmd->redirs);
	free_list(cmd->args);
	free(cmd->heredoc_file);
	free(cmd->error_message);
	free(cmd);
}
void free_cmds(Command *list) {
	while (list) {
		Command *next = list->next;
		if (!list->job)
			free_cmd(list);
		list = next;
	}
}

void free_garbage(Context *ctx) {
	// first close all fds then wait pids,
	// because if we do it in the reverse order then pipes will hang.
	Garbage *curr = ctx->garbage;
	while (curr) {
		if (curr->fd != -1)
			close(curr->fd);
		curr = curr->next;
	}

	while (ctx->garbage) {
		Garbage *next = ctx->garbage->next;
		if (ctx->garbage->fd == -1)
			waitpid(ctx->garbage->pid, NULL, WUNTRACED);
		free(ctx->garbage);
		ctx->garbage = next;
	}
}

void free_job(Job *job) {
	free_cmd(job->cmd);
	free(job);
}
void free_jobs(Job *job) {
	bool notified = false;

	while (job) {
		Job *next = job->next;

		if (!waitpid(job->pid, NULL, WNOHANG)) {
			if (!notified) {
				printf("dinosh: killing running jobs...\n");
				notified = true;
			}
			kill(job->pid, SIGKILL);
		}

		free_job(job);
		job = next;
	}
}

void free_node(Parser *node) {
	free(node->content);
	free(node);
}
void free_list(Parser *list) {
	while (list) {
		Parser *next = list->next;
		free_node(list);
		list = next;
	}
}

void free_one_hash(Hash *hash) {
	free(hash->key);
	free(hash->value);
	hash->key = NULL;
	hash->value = NULL;
}
void free_hash(Context *ctx) {
	for (int i = 0; i < ctx->hash_len; i++) {
		if (ctx->hash[i].value)
			free_one_hash(ctx->hash+i);
	}
	free(ctx->hash);
}

void free_alias(Alias *alias) {
	free(alias->name);
	free(alias->value);
	free(alias);
	alias = NULL;
}

void unalias_all(Context *ctx) {
	Alias *current = ctx->alias;
	Alias *temp;

	while(current) {
		temp = current;
		current = current->next;
		free_alias(temp);
	}
	ctx->alias = NULL;
}
