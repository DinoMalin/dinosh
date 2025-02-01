#include "minishell.h"

#define WORD_END "\t\n\v\f\r\"' <>|&"
#define ft_isspace(x) ((x >= '\t' && x <= '\r') || x == ' ')
#define IS_REDIR(x) (x == t_append || x == t_heredoc || x == t_to || x == t_from)
#define CAN_REDIR(x) (x == t_word || x == t_double_quotes || x == t_single_quotes)

#define ADD_TOKEN(head, curr, new)	\
	{								\
		if (!head) {				\
			curr = new;				\
			head = curr;			\
		} else {					\
			curr->next = new;		\
			curr = curr->next;		\
		}							\
	}

#define PARSE_TOKEN(start, end, type)						\
	{														\
		int len = ft_strlen(start);							\
		if (!ft_strncmp(str, start, len)) {					\
			str += len;										\
			Parser *new = ft_calloc(1, sizeof(Parser));			\
			new->content = until(&str, end);				\
			new->token = type;								\
			new->index = index;								\
			if (!new->content)								\
				new->error = unclosed_token;				\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

#define PARSE_OPERATOR(op, type)							\
	{														\
		if (!ft_strncmp(str, op, ft_strlen(op))) {			\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = operator(&str, op);				\
			new->token = type;								\
			index++;										\
			new->index = index;								\
			index++;										\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

#define PARSE_WORD()									\
	{													\
		char *content = parse_word(&str);				\
		if (!content)									\
			continue;									\
		Parser *new = ft_calloc(1, sizeof(Parser));		\
		new->content = content;							\
		new->token = t_word;							\
		new->index = index;								\
		ADD_TOKEN(head, curr, new);						\
		continue;										\
	}

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

#define TREAT_ERRORS(head)						\
	{											\
		if (error) {							\
			Command *new = init_cmd(from);		\
			ADD_COMMAND(head, new, curr);		\
		}										\
	}

#define PROCESS_TRANSMISSION(operator, type)	\
	{											\
		if (data->token == operator) {			\
			if (data_index == 0)				\
				error = unexpected_token;		\
			arg_index = 0;						\
			data_index = 0;						\
			if (curr)							\
				curr->to = type;				\
			from = type;						\
			data = data->next;					\
			continue;							\
		}										\
	}

void	 init_ac(Command *head);
void	 check_redir(Command *cmd, Parser **data);
void	 check_type(Command *cmd, Token token);
