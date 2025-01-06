#include "minishell.h"

int main(int ac, char **av, char **envp) {
	(void)ac;
	(void)av;
	tests_parsing(envp);

	char *prompt = "echo test > out | cat file > in | wc -l";
	Node *data = parse(prompt, envp);
	Command *cmd = process(data);
	free_list(data);

	execute(cmd);
	
	free_cmds(cmd);

}
