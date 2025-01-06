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
		COMMAND("echo", AV("test"), REDIRS(REDIR("in", r_from))),
		COMMAND("cat", NO_AV, REDIRS(REDIR("out", r_to)))
	}, envp));
	assert(comp("echo\t\r   test\t\t    <\t\tin\t|   cat    >  out", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(REDIR("in", r_from))),
		COMMAND("cat", NO_AV, REDIRS(REDIR("out", r_to)))
	}, envp));

	// Command
	assert(comp("echo test", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp("echo ", (Command[]) {
		COMMAND("echo", NO_AV, NO_REDIR)
	}, envp));
	assert(comp("echo", (Command[]) {
		COMMAND("echo", NO_AV, NO_REDIR)
	}, envp));
	assert(comp("", (Command[]) {
		COMMAND(NULL, NO_AV, NO_REDIR)
	}, envp));
	assert(comp("> out", (Command[]) {
		COMMAND(NULL, NO_AV, REDIRS(REDIR("out", r_to)))
	}, envp));
	assert(comp(">out", (Command[]) {
		COMMAND(NULL, NO_AV, REDIRS(REDIR("out", r_to)))
	}, envp));
	assert(comp("> out echo", (Command[]) {
		COMMAND("echo", NO_AV, REDIRS(REDIR("out", r_to)))
	}, envp));
	assert(comp(">out echo", (Command[]) {
		COMMAND("echo", NO_AV, REDIRS(REDIR("out", r_to)))
	}, envp));

	// Redir
	assert(comp("echo test", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));

	assert(comp("echo test < in", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(REDIR("in", r_from)))
	}, envp));
	assert(comp("echo test <in", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(REDIR("in", r_from)))
	}, envp));

	assert(comp("echo test > out", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(REDIR("out", r_to)))
	}, envp));
	assert(comp("echo test >out", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(REDIR("out", r_to)))
	}, envp));

	assert(comp("echo test >> out", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(REDIR("out", r_append)))
	}, envp));
	assert(comp("echo test >>out", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(REDIR("out", r_append)))
	}, envp));

	assert(comp("echo test << here", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(REDIR("here", r_heredoc)))
	}, envp));
	assert(comp("echo test <<here", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(REDIR("here", r_heredoc)))
	}, envp));

	// Multiple redirs
	assert(comp("echo test > out >> append", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(
			REDIR("out", r_to),
			REDIR("append", r_append)
		))
	}, envp));
	assert(comp("echo test < in << here", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(
			REDIR("in", r_from),
			REDIR("here", r_heredoc)
		))
	}, envp));
	assert(comp("echo test < in << here > out >> append", (Command[]) {
		COMMAND("echo", AV("test"), REDIRS(
			REDIR("in", r_from),
			REDIR("here", r_heredoc),
			REDIR("out", r_to),
			REDIR("append", r_append)
		))
	}, envp));

	assert(comp("echo test > ", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp("echo test >", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp("echo test < ", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp("echo test <", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp("echo test >> ", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp("echo test >>", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp("echo test << ", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp("echo test <<", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp(">>>", (Command[]) {
		COMMAND(NULL, NO_AV, NO_REDIR),
	}, envp));

	// Env var
	assert(comp("echo $USER", (Command[]) {
		COMMAND("echo", AV(getenv("USER")), NO_REDIR)
	}, envp));
	assert(comp("echo $LS_COLORS", (Command[]) {
		COMMAND("echo", AV(getenv("LS_COLORS")), NO_REDIR)
	}, envp));
	assert(comp("echo $UNKNOWN", (Command[]) { // must be non existant
		COMMAND("echo", AV(""), NO_REDIR)
	}, envp));
	assert(comp("echo '$USER'", (Command[]) {
		COMMAND("echo", AV("$USER"), NO_REDIR)
	}, envp));
	assert(comp("echo \"$USER\"", (Command[]) {
		COMMAND("echo", AV(getenv("USER")), NO_REDIR)
	}, envp));
	assert(comp("echo \"$XDG_\"", (Command[]) { // must be non existant
		COMMAND("echo", AV(""), NO_REDIR)
	}, envp));

	// Quotes
	assert(comp("echo \"test\"", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR)
	}, envp));
	assert(comp("echo \"test\"test", (Command[]) {
		COMMAND("echo", AV("testtest"), NO_REDIR)
	}, envp));
	assert(comp("echo 'test'test", (Command[]) {
		COMMAND("echo", AV("testtest"), NO_REDIR)
	}, envp));
	assert(comp("echo '\"test\"'", (Command[]) {
		COMMAND("echo", AV("\"test\""), NO_REDIR)
	}, envp));
	assert(comp("echo \"'test'\"", (Command[]) {
		COMMAND("echo", AV("'test'"), NO_REDIR)
	}, envp));
	assert(comp("echo \"test1 test2 test3\"", (Command[]) {
		COMMAND("echo", AV("test1 test2 test3"), NO_REDIR)
	}, envp));
	assert(comp("echo \"test1 test2 test3 \"test4", (Command[]) {
		COMMAND("echo", AV("test1 test2 test3 test4"), NO_REDIR)
	}, envp));

	// Pipes
	assert(comp("echo test | cat out", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR),
		COMMAND("cat", AV("out"), NO_REDIR)
	}, envp));
	assert(comp("echo test | ", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR),
	}, envp));
	assert(comp("echo test |", (Command[]) {
		COMMAND("echo", AV("test"), NO_REDIR),
	}, envp));
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
	char *input = malloc(size+1);
	ft_memcpy(input, data, size);
	input[size] = 0;

	Node *parsing_data = parse(input, (char*[]){NULL});
	Command *cmd = process(parsing_data);

	free_list(parsing_data);
	free_cmds(cmd);
	free(input);

	return 0;
}
