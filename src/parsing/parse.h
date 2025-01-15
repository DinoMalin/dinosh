#include "minishell.h"
#include <dirent.h>

#define ft_isspace(x) ((x >= '\t' && x <= '\r') || x == ' ')

#define CAN_EXPAND(x) ((x->token == t_double_quotes || x->token == t_word) && ft_strchr(curr->content, '$'))
#define CAN_WILDCARD(x) (curr->token == t_word && ft_strchr(curr->content, '*'))

#define IS_STAR(x) (x == '*' * -1)
#define EQUAL(x, e) (x && e && (x == e || (IS_STAR(x) && e == '*')))
#define NOT_EQUAL(x, e) (x != e && !(IS_STAR(x) && e == '*') && x != '*')

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
			Node *new = ft_calloc(1, sizeof(Node));			\
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
			Node *new = ft_calloc(1, sizeof(Node));		\
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
		Node *new = ft_calloc(1, sizeof(Node));		\
		new->content = content;						\
		new->token = t_word;							\
		new->index = index;							\
		ADD_TOKEN(head, curr, new);					\
		continue;									\
	}

#define ADD_SPECIAL_VAR(var)			\
{										\
	int len = ft_strlen(var);			\
	if (!ft_strncmp(var, str, len))		\
		return len;						\
}

Node	*tokenize(char *str);
char	*expand(char *str, char **envp);
char	*expand_wildcard(char *str);
