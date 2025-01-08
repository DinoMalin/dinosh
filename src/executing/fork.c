#include "minishell.h"

int g_exit_status = 0;

void exit_fork(int exit_code, Command *cmd, char **env) {
	free_cmds(cmd);
	free_av(env);
	exit(exit_code);
}

void redirect_pipe(int *pipe_fd, int end) {
	if (dup2(pipe_fd[end], end) < 0)
		perror("dinosh: dup2");
	if (end && close(pipe_fd[0]) < 0)
		perror("dinosh: close 0");
	if (!end && close(pipe_fd[1]) < 0)
		perror("dinosh: close 1");
}

void fork_routine(Command *head, Command *cmd, char **env, int *pipe_fd) {
	redirect(cmd);
	if (cmd->next)
		redirect_pipe(pipe_fd, 1);

	if (!IS_BUILTIN(cmd->type)) {
		char *path = find_path(env, cmd->cmd);

		if (path)
			execve(path, cmd->av, env);
	} else {
		printf("%s is a builtin\n", cmd->cmd);
	}

	exit_fork(0, head, env);
}

void create_fork(Command *head, Command *cmd, char **env, int *pipe_fd) {
	pid_t pid = fork();

	if (pid == -1)
		perror("dinosh: fork");
	else if (IS_CHILD(pid))
		fork_routine(head, cmd, env, pipe_fd);
	else if (cmd->next)
		redirect_pipe(pipe_fd, 0);
}

void execute(Command *head, char **env) {
	Command *curr = head;
	int pipe_fd[2];

	while (curr) {
		if (curr->next && pipe(pipe_fd) < 0)
			perror("dinosh: pipe");
		create_fork(head, curr, env, pipe_fd);
		curr = curr->next;
	}

	curr = head;
	while (curr) {
		wait(&g_exit_status);
		curr = curr->next;
	}
}
