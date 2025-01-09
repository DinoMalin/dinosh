#include "minishell.h"
#include "tests.h"

int comp_av(char **av1, char **av2) {
	int i;
	for (i = 0; av1[i] && av2[i]; i++) {
		if (ft_strcmp(av1[i], av2[i]))
			return false;
	}
	if (av1[i] || av2[i])
		return false;
	return true;
}

int comp_redir(t_redir *r1, t_redir *r2) {
	int i;
	for (i = 0; r1[i].file && r2[i].file; i++) {
		if (ft_strcmp(r1[i].file, r2[i].file))
			return false;
		if (r1[i].type != r2[i].type)
			return false;
	}
	if (r1[i].file || r2[i].file)
		return false;
	return true;
}

int comp_cmd(Command *cmd1, Command *cmd2) {
	if (STRINGS_DIFFER(cmd1->cmd, cmd2->cmd))
		FAIL("wrong cmd->cmd");
	if (!comp_av(cmd1->av, cmd2->av))
		FAIL("wrong cmd->av");
	if (!comp_redir(cmd1->redirs, cmd2->redirs))
		FAIL("wrong cmd->redirs");

	return 1;
}

int comp(char *prompt, Command *expected, char **envp) {
	Node *data = parse(prompt, envp);
	if (parsing_errors(data)) {
		free_list(data);
		return 0;
	}

	Command *cmd = process(data);
	Command *curr = cmd;
	int i = 0;

	while (curr) {
		if (!comp_cmd(curr, expected+i)) {
			free_list(data);
			free_cmds(cmd);
			return 0;
		}
		i++;
		curr = curr->next;
	}

	free_list(data);
	free_cmds(cmd);
	return 1;
}

void tests_parsing(char **envp) {
	// Basic
	assert(comp("echo test < in | cat > out", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("in", r_from)), ECHO),
		COMMAND("cat", AV("cat"), REDIRS(REDIR("out", r_to)), BASIC)
	}, envp));
	assert(comp("echo\t\r   test\t\t    <\t\tin\t|   cat    >  out", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("in", r_from)), ECHO),
		COMMAND("cat", AV("cat"), REDIRS(REDIR("out", r_to)), BASIC)
	}, envp));

	// Command
	assert(comp("echo test", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo ", (Command[]) {
		COMMAND("echo", AV("echo"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo", (Command[]) {
		COMMAND("echo", AV("echo"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("", (Command[]) {
		COMMAND(NULL, NO_AV, NO_REDIR, BASIC)
	}, envp));
	assert(comp("> out", (Command[]) {
		COMMAND(NULL, NO_AV, REDIRS(REDIR("out", r_to)), BASIC)
	}, envp));
	assert(comp(">out", (Command[]) {
		COMMAND(NULL, NO_AV, REDIRS(REDIR("out", r_to)), BASIC)
	}, envp));
	assert(comp("> out echo", (Command[]) {
		COMMAND("echo", AV("echo"), REDIRS(REDIR("out", r_to)), BASIC)
	}, envp));
	assert(comp(">out echo", (Command[]) {
		COMMAND("echo", AV("echo"), REDIRS(REDIR("out", r_to)), BASIC)
	}, envp));

	// Redir
	assert(comp("echo test", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));

	assert(comp("echo test < in", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("in", r_from)), ECHO)
	}, envp));
	assert(comp("echo test <in", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("in", r_from)), ECHO)
	}, envp));

	assert(comp("echo test > out", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("out", r_to)), ECHO)
	}, envp));
	assert(comp("echo test >out", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("out", r_to)), ECHO)
	}, envp));

	assert(comp("echo test >> out", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("out", r_append)), ECHO)
	}, envp));
	assert(comp("echo test >>out", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("out", r_append)), ECHO)
	}, envp));

	assert(comp("echo test << here", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("here", r_heredoc)), ECHO)
	}, envp));
	assert(comp("echo test <<here", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(REDIR("here", r_heredoc)), ECHO)
	}, envp));

	// Multiple redirs
	assert(comp("echo test > out >> append", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(
			REDIR("out", r_to),
			REDIR("append", r_append)
		), ECHO)
	}, envp));
	assert(comp("echo test < in << here", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(
			REDIR("in", r_from),
			REDIR("here", r_heredoc)
		), ECHO)
	}, envp));
	assert(comp("echo test < in << here > out >> append", (Command[]) {
		COMMAND("echo", AV("echo", "test"), REDIRS(
			REDIR("in", r_from),
			REDIR("here", r_heredoc),
			REDIR("out", r_to),
			REDIR("append", r_append)
		), ECHO)
	}, envp));

	assert(comp("echo test > ", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo test >", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo test < ", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo test <", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo test >> ", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo test >>", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo test << ", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo test <<", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp(">>>", (Command[]) {
		COMMAND(NULL, NO_AV, NO_REDIR, BASIC),
	}, envp));

	// Env var
	assert(comp("echo $USER", (Command[]) {
		COMMAND("echo", AV("echo", getenv("USER")), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo $LS_COLORS", (Command[]) {
		COMMAND("echo", AV("echo", getenv("LS_COLORS")), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo $UNKNOWN", (Command[]) { // must be non existant
		COMMAND("echo", AV("echo", ""), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo '$USER'", (Command[]) {
		COMMAND("echo", AV("echo", "$USER"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo \"$USER\"", (Command[]) {
		COMMAND("echo", AV("echo", getenv("USER")), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo \"$XDG_\"", (Command[]) { // must be non existant
		COMMAND("echo", AV("echo", ""), NO_REDIR, ECHO)
	}, envp));

	// Quotes
	assert(comp("echo \"test\"", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo \"test\"test", (Command[]) {
		COMMAND("echo", AV("echo", "testtest"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo 'test'test", (Command[]) {
		COMMAND("echo", AV("echo", "testtest"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo '\"test\"'", (Command[]) {
		COMMAND("echo", AV("echo", "\"test\""), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo \"'test'\"", (Command[]) {
		COMMAND("echo", AV("echo", "'test'"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo \"test1 test2 test3\"", (Command[]) {
		COMMAND("echo", AV("echo", "test1 test2 test3"), NO_REDIR, ECHO)
	}, envp));
	assert(comp("echo \"test1 test2 test3 \"test4", (Command[]) {
		COMMAND("echo", AV("echo", "test1 test2 test3 test4"), NO_REDIR, ECHO)
	}, envp));

	// Pipes
	assert(comp("echo test | cat out", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO),
		COMMAND("cat", AV("cat", "out"), NO_REDIR, BASIC)
	}, envp));
	assert(comp("echo test | ", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO),
	}, envp));
	assert(comp("echo test |", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO),
	}, envp));

	// Builtins
	assert(comp("cat test", (Command[]) {
		COMMAND("cat", AV("cat", "test"), NO_REDIR, BASIC),
	}, envp));
	assert(comp("echo test", (Command[]) {
		COMMAND("echo", AV("echo", "test"), NO_REDIR, ECHO),
	}, envp));
	assert(comp("cd test", (Command[]) {
		COMMAND("cd", AV("cd", "test"), NO_REDIR, CD),
	}, envp));
	assert(comp("pwd test", (Command[]) {
		COMMAND("pwd", AV("pwd", "test"), NO_REDIR, PWD),
	}, envp));
	assert(comp("export test", (Command[]) {
		COMMAND("export", AV("export", "test"), NO_REDIR, EXPORT),
	}, envp));
	assert(comp("unset test", (Command[]) {
		COMMAND("unset", AV("unset", "test"), NO_REDIR, UNSET),
	}, envp));
	assert(comp("env test", (Command[]) {
		COMMAND("env", AV("env", "test"), NO_REDIR, ENV),
	}, envp));
	assert(comp("exit test", (Command[]) {
		COMMAND("exit", AV("exit", "test"), NO_REDIR, EXIT),
	}, envp));
	
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	char *input = malloc(size+1);
	ft_memcpy(input, data, size);
	input[size] = 0;

	Node *parsing_data = parse(input, (char*[]){NULL});

	if (parsing_errors(parsing_data)) {
		free_list(parsing_data);
		free(input);
		return 0;
	}

	Command *cmd = process(parsing_data);

	free_list(parsing_data);
	free_cmds(cmd);
	free(input);

	return 0;
}
