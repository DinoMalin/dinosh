#include "minishell.h"

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;
	tests_parsing(envp);

	char *prompt = "sleep 2 | sleep 2";
	char **new_env = copy_env(envp);

	Node *data = parse(prompt, new_env);
	Command *cmd = process(data);
	free_list(data);

	execute(cmd, new_env);
	
	free_cmds(cmd);
	free_av(new_env);
}
