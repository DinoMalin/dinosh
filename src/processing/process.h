#include "minishell.h"

#define is_redir(x) (x == t_append || x == t_heredoc || x == t_to || x == t_from)

#define ADD_COMMAND(head, new, last)		\
	{										\
		if (!head)							\
			head = new;						\
		else								\
			last->next = new;				\
	}

void	 init_ac(Command *head);
void	 check_redir(Command *cmd, Node **data);
void	 check_type(Command *cmd, Token token);
