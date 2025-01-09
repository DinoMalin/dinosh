#include "minishell.h"

void handle_prompt(char *prompt, char **envp) {
	Node *data = parse(prompt, envp);
	if (parsing_error(data)) {
		free_list(data);
		return;
	}

	Command *cmd = process(data);
	free_list(data);

	if (process_error(cmd)) {
		free_cmds(cmd);
		return;
	}

	execute(cmd, envp);
	free_cmds(cmd);
}

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;

	char **new_env = copy_env(envp);
	tests_parsing(envp);

	char *prompt = "(cat Makefile | wc -l) > out";
	handle_prompt(prompt, new_env);

	free_av(new_env);
}
