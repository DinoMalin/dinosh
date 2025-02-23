#include "minishell.h"

#define WORD_END "\t\n\v\f\r\"' <>|&()*;\\"
#define MINI_WORD_END "\t\n\v\f\r "
#define ESCAPED	"\""
#define CAN_REDIR(x) (x == t_word || x == t_wildcard || x == t_double_quotes || x == t_single_quotes)

#define BUILTIN(s, type)					\
	{										\
		if (!ft_strcmp(s, name))			\
			return type;					\
	}

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
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = until(&str, end);				\
			new->token = type;								\
			new->id = id;									\
			if (!new->content)								\
				new->error = unclosed_token;				\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

#define PARSE_ESCAPED_TOKEN(start, end, type)				\
	{														\
		int len = ft_strlen(start);							\
		if (!ft_strncmp(str, start, len)) {					\
			str += len;										\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = until_escaped(&str, end);		\
			new->token = type;								\
			new->id = id;									\
			if (!new->content)								\
				new->error = unclosed_token;				\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

// A character that does not belong to the same arg
#define PARSE_OPERATOR(op, type)							\
	{														\
		if (!ft_strncmp(str, op, ft_strlen(op))) {			\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = operator(&str, op);				\
			new->token = type;								\
			id++;											\
			new->id = id;									\
			id++;											\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

// will check only X character before start
#define PARSE_OPERATOR_BPARAMETER(op, len, type, next_arg)	\
	{														\
		int l = ft_strlen(op);								\
		if (ft_strlen(str) >= (size_t)len+l					\
			&& !ft_strncmp(str+len, op, l)) {				\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = until(&str, op);					\
			new->token = type;								\
			if (next_arg)									\
				id++;										\
			new->id = id;									\
			if (next_arg)									\
				id++;										\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

// will check only X character after start
#define PARSE_OPERATOR_APARAMETER(op, len, type, next_arg)	\
	{														\
		int l = ft_strlen(op);								\
		if (!ft_strncmp(str, op, l)) {						\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = ft_substr(str, 1, len);			\
			str += len+l;									\
			new->token = type;								\
			if (next_arg)									\
				id++;										\
			new->id = id;									\
			if (next_arg)									\
				id++;										\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

// A character that belongs to the same arg
#define PARSE_CHARACTER(ch, type)							\
	{														\
		if (!ft_strncmp(str, ch, ft_strlen(ch))) {			\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = operator(&str, ch);				\
			new->token = type;								\
			new->id = id;									\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

#define PARSE_WORD(word_end)							\
	{													\
		char *content = parse_word(&str, word_end);		\
		if (!content)									\
			continue;									\
		Parser *new = ft_calloc(1, sizeof(Parser));		\
		new->content = content;							\
		new->token = t_word;							\
		new->id = id;									\
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

#define PROCESS_TRANSMISSION(operator, type, dependant)		\
	{														\
		if (data->token == operator) {						\
			if (data_index == 0)							\
				error = unexpected_token;					\
			if (dependant && !data->next)					\
				error = unexpected_token;					\
			arg_index = 0;									\
			data_index = 0;									\
			if (curr)										\
				curr->to = type;							\
			from = type;									\
			data = data->next;								\
			continue;										\
		}													\
	}

void	check_redir_errors(Command *cmd, Parser **data);
void	check_type(Command *cmd, Token token);
