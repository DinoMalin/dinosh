#include "execute.h"

void init_av(Command *cmd) {
	Parser *curr = cmd->args;

	while (curr) {
		Parser *next = curr->next;
		cmd->av = clean_strsjoin(cmd->av, ft_strdup(curr->content));
		curr = next;
		cmd->ac++;
	}
}

void add_redir(Command *cmd, Token type, char *name) {
	Redir r_type = 0;
	if (type == t_from)
		r_type = r_from;
	else if (type == t_heredoc)
		r_type = r_heredoc;
	else if (type == t_to)
		r_type = r_to;
	else if (type == t_append)
		r_type = r_append;

	t_redir redirection = (t_redir){ft_strdup(name), r_type, 0, 0};
	cmd->redirs = clean_redirjoin(cmd->redirs, redirection);
}

void add_redir_to_fd(Command *cmd, char *in, char *out) {
	if (!is_number(in) || !is_number(out)) {
		dprintf(2, "dinosh: numeric argument required\n");
		cmd->error = numeric_argument;
		return;
	}
	int fd_in = ft_atoi(in);
	int fd_out = ft_atoi(out);
	t_redir redirection = (t_redir){ft_strdup(""), r_to_fd, fd_in, fd_out};
	cmd->redirs = clean_redirjoin(cmd->redirs, redirection);
}

// I don't check if file exists because it's supposed to be checked in parse()
void init_redirs(Command *cmd) {
	Parser *curr = cmd->args;
	Parser *prec = curr;

	while (curr) {
		if (IS_REDIR(curr->token)) {
			Parser *file = curr->next;
			if (file->next && IS_AMBIGUOUS(file)) {
				dprintf(2, "dinosh: ambiguous redirect\n");
				cmd->error = ambiguous_redirect;
			}
			if (curr->token == t_to_fd) {
				Parser *next = file->next;
				add_redir_to_fd(cmd, curr->content, file->content);
				DELETE_ARG(cmd->args, curr, prec);
				DELETE_ARG(cmd->args, file, prec);
				curr = next;
			} else {
				Parser *next = file->next;
				add_redir(cmd, curr->token, file->content);
				DELETE_ARG(cmd->args, curr, prec);
				DELETE_ARG(cmd->args, file, prec);
				curr = next;
			}
			continue;
		}
		prec = curr;
		curr = curr->next;
	}
}

void fill_heredoc(Command *cmd) {
	Parser *curr = cmd->args;
	while (curr) {
		if (curr->token == t_heredoc) {
			unlink(HEREDOC_FILE);
			Parser *file = curr->next;
			merge_one_node(file);
			if (!heredoc(file->content)) {
				cmd->error = eheredoc;
				return;
			}
		}
		curr = curr->next;
	}
}

bool init_command(Context *ctx, Command *cmd) {
	fill_heredoc(cmd);
	expand(cmd, ctx->env);
	init_redirs(cmd);
	if (command_error(cmd)) {
		cmd->pid = 0;
		return false;
	}

	merge(cmd->args);
	init_av(cmd);
	return true;
}
