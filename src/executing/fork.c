#include "minishell.h"

void exit_fork(int exit_code, Command *cmd, char **env) {
	free_cmds(cmd);
	free_av(env);
	exit(exit_code);
}

void fork_routine(Command *head, Command *cmd, char **env) {
	char *path = find_path(env, cmd->cmd);

	if (path)
		execve(path, cmd->av, env);

	exit_fork(0, head, env);
}

void create_fork(Command *head, Command *cmd, char **env) {
	pid_t pid = fork();

	if (pid == -1)
		perror("fork");
	else if (IS_CHILD(pid))
		fork_routine(head, cmd, env);
}

void execute(Command *head, char **env) {
	Command *curr = head;
	while (curr) {
		create_fork(head, curr, env);
		curr = curr->next;
	}
}
