#include "minishell.h"

void exit_fork(int exit_code, Command *cmd) {
	free_cmds(cmd);
	exit(exit_code);
}

void fork_routine(Command *head, Command *cmd) {
	printf("%s\n", cmd->cmd);
	for (int i = 0; cmd->av[i]; i++) {
		printf("%s ", cmd->av[i]);
	}
	printf("\n\n");
	exit_fork(0, head);
}

void create_fork(Command *head, Command *cmd) {
	pid_t pid = fork();

	if (pid == -1)
		perror("fork");
	else if (IS_CHILD(pid))
		fork_routine(head, cmd);
}

void execute(Command *head) {
	Command *curr = head;
	while (curr) {
		create_fork(head, curr);
		curr = curr->next;
	}
}
