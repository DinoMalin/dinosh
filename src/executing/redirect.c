#include "execute.h"

void redirect_pipe(Command *cmd, Pipes *pipes) {
	if (cmd->from == PIPE) {
		if (dup2(pipes->prev[0], 0) < 0)
			perror("dinosh: dup2");
		close(pipes->prev[0]);
	}
	if (cmd->to == PIPE) {
		if (dup2(pipes->curr[1], 1) < 0)
			perror("dinosh: dup2");
		close(pipes->curr[1]);
	}

	if (pipes->prev[1] != -1) close(pipes->prev[1]);
	if (pipes->curr[0] != -1) close(pipes->curr[0]);
}

void fd_storage(StorageAction action) {
	static int fd_stdin = -1;
	static int fd_stdout = -1;

	if (action == STORE) {
		fd_stdin = dup(0);
		if (fd_stdin < 0)
			perror("dinosh: dup");
		fd_stdout = dup(1);
		if (fd_stdout < 0)
			perror("dinosh: dup");
	} else {
		if (dup2(fd_stdin, 0) < 0)
			perror("dinosh: dup2");
		if (dup2(fd_stdout, 1) < 0)
			perror("dinosh: dup2");
		xclose(fd_stdin);
		xclose(fd_stdout);
	}
}

bool handle_redir(char *file, int *fd, int flags, int dup) {
	if (*fd > 2)
		close(*fd);
	*fd = open(file, flags, 0644);
	if (*fd < 0)
		return false;
	if (dup2(*fd, dup) < 0)
		return false;
	close(*fd);
	return true;
}

int heredoc(char *heredoc_file, char *lim) {
	int fd = open(heredoc_file, TO_FLAGS, 0644);
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
				cmd->error = eopen;
				return;
			}
		} else if (cmd->redirs[i].type == r_append) {
			if (!handle_redir(cmd->redirs[i].file, &fd_out, APPEND_FLAGS, 1)) {
				perror("dinosh: open");
				cmd->error = eopen;
				return;
			}
		} else if (cmd->redirs[i].type == r_from) {
			if (!handle_redir(cmd->redirs[i].file, &fd_in, FROM_FLAGS, 0)) {
				perror("dinosh: open");
				cmd->error = eopen;
				return;
			}
		} else if (cmd->redirs[i].type == r_heredoc) {
			if (!handle_redir(cmd->heredoc_file, &fd_in, FROM_FLAGS, 0)) {
				perror("dinosh: open");
				cmd->error = eopen;
				return;
			}
			unlink(cmd->heredoc_file);
		} else if (cmd->redirs[i].type == r_to_fd) {
			if (cmd->redirs[i].word == -1)
				close(cmd->redirs[i].n);
			else if (dup2(cmd->redirs[i].word, cmd->redirs[i].n) < 0) {
				perror("dinosh: dup2");
				cmd->error = eopen;
				return;
			}
		} else if (cmd->redirs[i].type == r_from_fd) {
			if (cmd->redirs[i].word == -1)
				close(cmd->redirs[i].n);
			else if (dup2(cmd->redirs[i].word, cmd->redirs[i].n) < 0) {
				perror("dinosh: dup2");
				cmd->error = eopen;
				return;
			}
		}
	}
}
