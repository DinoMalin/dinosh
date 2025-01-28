#include "minishell.h"

#define ft_isspace(x) ((x >= '\t' && x <= '\r') || x == ' ')

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

#define PARSE_OPERATOR(op, type)						\
	{													\
		if (!ft_strncmp(str, op, ft_strlen(op))) {		\
			Parser *new = ft_calloc(1, sizeof(Parser));		\
			new->content = operator(&str, op);			\
			new->token = type;							\
			new->index = index;							\
			ADD_TOKEN(head, curr, new);					\
			index++;									\
			continue;									\
		}												\
	}

#define WORD_END "\t\n\v\f\r\"' "
#define PARSE_WORD()								\
	{												\
		char *content = parse_word(&str);			\
		if (!content)								\
			continue;								\
		Parser *new = ft_calloc(1, sizeof(Parser));		\
		new->content = content;						\
		new->token = t_word;							\
		new->index = index;							\
		ADD_TOKEN(head, curr, new);					\
		continue;									\
	}
