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

	t_redir redirection = (t_redir){ft_strdup(name), r_type};
	cmd->redirs = clean_redirjoin(cmd->redirs, redirection);
}

// I don't check if file exists because it's supposed to be checked in parse()
void init_redirs(Command *cmd) {
	Parser *curr = cmd->args;
	Parser *prec = curr;

	while (curr) {
		if (IS_REDIR(curr->token)) {
			Parser *file = curr->next;
			if (file->next && file->next->expand_id == file->expand_id && file->expand_id != -1) {
				dprintf(2, "dinosh: ambiguous redirect\n"); // print this elsewhere
				cmd->error = ambiguous_redirect;
			}
			Token token = curr->token;
			Parser *next = file->next;
			DELETE_ARG(cmd->args, curr, prec);
			add_redir(cmd, token, file->content);
			DELETE_ARG(cmd->args, file, prec);
			curr = next;
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
