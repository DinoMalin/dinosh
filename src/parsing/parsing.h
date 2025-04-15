#include "minishell.h"

#define WORD_END "\"'\\<>|&()*;$"
#define DOUBLE_END "\"$"
#define SINGLE_END "\'"
#define ESCAPED	"\""
#define CAN_REDIR(x) (x == t_word || x == t_wildcard || x == t_wordvar) // need to be more complete
#define IS_WILDCARD(x) (x == '*' || x == '?' || x == '[' || x == '!')

#define BUILTIN(s, type)					\
	{										\
		if (!ft_strcmp(s, name))			\
			return type;					\
	}

#define ADD_SPECIAL_VAR(var)				\
	{										\
		int len = ft_strlen(var);			\
		if (!ft_strncmp(var, str, len))		\
			return len;						\
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
			new->quoting = ADAPTED_QUOTING();				\
			new->token = type;								\
			new->id = id;									\
			if (!new->content)								\
				new->error = unclosed_token;				\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

#define PARSE_LEN(len, type, e)							\
	{													\
		Parser *new = ft_calloc(1, sizeof(Parser));		\
		new->content = ft_substr(str, 0, 1);			\
		str += len;										\
		new->quoting = ADAPTED_QUOTING();				\
		new->escaped = e;								\
		new->token = type;								\
		new->id = id;									\
		ADD_TOKEN(head, curr, new);						\
		continue;										\
	}

#define PARSE_VARIABLE(type)								\
	{														\
		if (!ft_strncmp(str, "$", 1)) {						\
			int len = get_variable_size(str+1);				\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = ft_substr(str, 1, len);			\
			str += len+1;									\
			new->quoting = ADAPTED_QUOTING();				\
			new->token = type;								\
			new->id = id;									\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

#define PARSE_TOKEN_NESTED(start, end, type)				\
	{														\
		int len = ft_strlen(start);							\
		if (!ft_strncmp(str, start, len)) {					\
			str += len;										\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = until_nested(&str, start, end);	\
			new->quoting = ADAPTED_QUOTING();				\
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
			new->quoting = ADAPTED_QUOTING();				\
			new->token = type;								\
			id++;											\
			new->id = id;									\
			id++;											\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

#define PARSE_OPERATOR_PARAMETER(op, len, type, next_arg)	\
	{														\
		int l = ft_strlen(op);								\
		if (ft_strlen(str) >= (size_t)len+l					\
			&& !ft_strncmp(str+len, op, l)) {				\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = until(&str, op);					\
			new->quoting = ADAPTED_QUOTING();				\
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
			new->quoting = ADAPTED_QUOTING();				\
			new->token = type;								\
			new->id = id;									\
			ADD_TOKEN(head, curr, new);						\
			continue;										\
		}													\
	}

#define PARSE_WORD()									\
	{													\
		char *content = parse_word(&str, WORD_END,		\
							 ADAPTED_QUOTING());		\
		if (!content)									\
			continue;									\
		Parser *new = ft_calloc(1, sizeof(Parser));		\
		new->content = content;							\
		new->quoting = ADAPTED_QUOTING();				\
		new->token = t_word;							\
		new->id = id;									\
		ADD_TOKEN(head, curr, new);						\
		continue;										\
	}

#define PARSE_MINI()										\
	{														\
		char *content = parse_word(&str, "", none);			\
		if (!content)										\
			continue;										\
		Parser *new = ft_calloc(1, sizeof(Parser));			\
		new->content = content;								\
		new->token = t_word;								\
		new->id = id;										\
		ADD_TOKEN(head, curr, new);							\
		continue;											\
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

#define ADAPTED_QUOTING()			\
	(								\
		double_quotes ? doubles : 	\
		single_quotes ? singles :	\
						none		\
	)

#define CHECK_QUOTING()								\
	{												\
		if (*str == '"') {							\
			if (!single_quotes)						\
				double_quotes = !double_quotes;		\
			str++;									\
		}											\
		if (*str == '\'') {							\
			if (!double_quotes)						\
				single_quotes = !single_quotes;		\
			str++;									\
		}											\
		if (*str == '\\') {							\
			escape = true;							\
			str++;									\
		}											\
	}

void	check_redir_errors(Command *cmd, Parser **data);
void	check_type(Command *cmd, Token token);
