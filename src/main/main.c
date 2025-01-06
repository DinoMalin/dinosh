#include "minishell.h"

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;
	tests_parsing(envp);

	char *prompt = "echo test | cat Makefile | ls | nonexistant";
	char **new_env = copy_env(envp);

	Node *data = parse(prompt, new_env);
	Command *cmd = process(data);
	free_list(data);

	execute(cmd, new_env);
	
	free_cmds(cmd);
	free_av(new_env);
}
