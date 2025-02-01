#include "minishell.h"
#include <dirent.h>

#define CAN_EXPAND(x) ((x->token == t_double_quotes || x->token == t_word) && ft_strchr(curr->content, '$'))
#define CAN_WILDCARD(x) (curr->token == t_word && ft_strchr(curr->content, '*'))

#define IS_STAR(x) (x == '*' * -1)
#define EQUAL(x, e) (x && e && (x == e || (IS_STAR(x) && e == '*')))
#define NOT_EQUAL(x, e) (x != e && !(IS_STAR(x) && e == '*') && x != '*')

#define ADD_SPECIAL_VAR(var)				\
	{										\
		int len = ft_strlen(var);			\
		if (!ft_strncmp(var, str, len))		\
			return len;						\
	}


#define REATTRIBUTE_INDEXES()				\
	{										\
		for (int i = 0; curr; i++) {		\
			max = max+i;					\
			if (ft_strlen(el->content))		\
				max++;						\
			curr->index = max;				\
			curr = curr->next;				\
		}									\
	}

#define MERGE_FIRST_NODE()											\
	{																\
		Parser *next = args->next;									\
		el->content = clean_join(el->content, args->content);		\
		free_node(args);											\
		args = next;												\
	}

#define INSERT_NEW_ARGS()			\
	{								\
		Parser *next = el->next;	\
		el->next = args;			\
		while (el->next) {			\
			el = el->next;			\
		}							\
		el->next = next;			\
	}

#define TOKENIZE_VAR()							\
	{											\
		Parser *args = tokenize(value);			\
		Parser *curr = args;					\
		REATTRIBUTE_INDEXES();					\
		if (!ft_isspace(value[0]) && args) {	\
			MERGE_FIRST_NODE();					\
		}										\
		INSERT_NEW_ARGS();						\
	}

void	expand_vars(Env *env, Parser *el, int max);
Parser	*expand_wildcard(Parser *el, int max);
