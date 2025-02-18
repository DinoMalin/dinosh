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
	free(cmd);
}

void free_cmds(Command *list) {
	while (list) {
		Command *next = list->next;
		if (list->to != BACKGROUND)
			free_cmd(list);
		list = next;
	}
}

void free_jobs(Job *job) {
	while (job) {
		Job *next = job->next;
		free_cmd(job->cmd);
		free(job);
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
