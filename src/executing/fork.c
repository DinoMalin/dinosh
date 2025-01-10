#include "minishell.h"

int g_exit_status = 0;

void exit_fork(int exit_code, Command *cmd, Prompt *prompt) {
	free_cmds(cmd);
	free_av(prompt->env);
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

void fork_routine(Command *head, Command *cmd, Prompt *prompt, int *pipe_fd) {
	redirect(cmd);
	if (cmd->next)
		redirect_pipe(pipe_fd, 1);

	if (IS_BUILTIN(cmd->type))
		builtin(cmd, prompt);
	else if (cmd->type == SUBSHELL) {
		Prompt subprompt = {cmd->cmd, prompt->env};
		handle_prompt(&subprompt); // todo: create copy of env
	} else if (cmd->type == BASIC) {
		char *path = find_path(prompt->env, cmd->cmd);

		if (path)
			execve(path, cmd->av, prompt->env);
	}

	exit_fork(0, head, prompt);
}

void create_fork(Command *head, Command *cmd, Prompt *prompt, int *pipe_fd) {
	pid_t pid = fork();

	if (pid == -1)
		perror("dinosh: fork");
	else if (IS_CHILD(pid))
		fork_routine(head, cmd, prompt, pipe_fd);
	else if (cmd->next)
		redirect_pipe(pipe_fd, 0);
}

void execute(Command *head, Prompt *prompt) {
	Command *curr = head;
	int pipe_fd[2];

	while (curr) {
		if (!IS_BUILTIN(curr->type) || curr->transmission == PIPE) {
			if (curr->next && pipe(pipe_fd) < 0)
				perror("dinosh: pipe");
			create_fork(head, curr, prompt, pipe_fd);
		} else if (IS_BUILTIN(curr->type))
			builtin(curr, prompt);
		curr = curr->next;
	}

	curr = head;
	while (curr) {
		wait(&g_exit_status);
		curr = curr->next;
	}
}
