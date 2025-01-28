#include "minishell.h"

#define is_redir(x) (x == t_append || x == t_heredoc || x == t_to || x == t_from)

#define ADD_COMMAND(head, new, last)		\
	{										\
		if (!head)							\
			head = new;						\
		else								\
			last->next = new;				\
		if (error) {						\
			new->error = error;				\
			error = no_error;				\
		}									\
	}

#define PROCESS_TRANSMISSION(operator, type)	\
	{											\
		if (data->token == operator) {			\
			arg_index = 0;						\
			data_index = 0;						\
			if (curr)							\
				curr->to = type;				\
			else								\
				error = start_pipe;				\
			from = type;						\
			data = data->next;					\
			continue;							\
		}										\
	}

void	 init_ac(Command *head);
void	 check_redir(Command *cmd, Parser **data);
void	 check_type(Command *cmd, Token token);
