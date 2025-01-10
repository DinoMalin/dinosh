#include "minishell.h"

int handle_redir(char *file, int *fd, int flags, int dup) {
	if (*fd > 2)
		close(*fd);
	*fd = open(file, flags, 0644);
	if (*fd < 0)
		return 0;
	if (dup2(*fd, dup) < 0)
		return 0;
	return 1;
}

int heredoc(char *lim) {
	int fd = open(HEREDOC_FILE, TO_FLAGS, 0644);
	if (fd < 0)
		return 0;

	char *line = NULL;
	bool keep_going = false;
	do {
		line = readline("> ");
		keep_going = line && ft_strcmp(lim, line);
		if (keep_going && dprintf(fd, "%s\n", line) < 0)
			return 0;
		free(line);
	} while (keep_going);

	close(fd);
	return 1;
}

void redirect(Command *cmd) {
	int fd_in = 0;
	int fd_out = 1;

	for (int i = 0; cmd->redirs[i].file; i++) {
		if (cmd->redirs[i].type == r_to) {
			if (!handle_redir(cmd->redirs[i].file, &fd_out, TO_FLAGS, 1)) {
				perror("dinosh: open");
				return;
			}
		} else if (cmd->redirs[i].type == r_append) {
			if (!handle_redir(cmd->redirs[i].file, &fd_out, APPEND_FLAGS, 1)) {
				perror("dinosh: open");
				return;
			}
		} else if (cmd->redirs[i].type == r_from) {
			if (!handle_redir(cmd->redirs[i].file, &fd_in, FROM_FLAGS, 0)) {
				perror("dinosh: open");
				return;
			}
		} else if (cmd->redirs[i].type == r_heredoc) {
			if (!heredoc(cmd->redirs[i].file)) {
				perror("dinosh: heredoc");
				return;
			}
			if (!handle_redir(HEREDOC_FILE, &fd_in, FROM_FLAGS, 0)) {
				perror("dinosh: open");
				return;
			}
		}
	}
}
