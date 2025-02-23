#include "minishell.h"
#include <dirent.h>

#define CAN_EXPAND(x) (((x->token == t_double_quotes || x->token == t_word) && ft_strchr(x->content, '$') && x->expand_id != -1) || x->token == t_var)

#define SAME_ID(x, y) (x && x->id == y->id)
#define IS_WILDCARD(x, y) (SAME_ID(x, y) && x->token == t_wildcard)
#define INCREMENT_CONTENT(curr, index)	\
	{									\
		index++;						\
		if (!curr->content[index]) {	\
			curr = curr->next;			\
			index = 0;					\
		}								\
	}

#define ADD_SPECIAL_VAR(var)				\
	{										\
		int len = ft_strlen(var);			\
		if (!ft_strncmp(var, str, len))		\
			return len;						\
	}

#define REATTRIBUTE_ID()											\
	{																\
		for (int i = 0; curr; i++) {								\
			curr->id = max+i+(ft_strlen(curr->content) > 0);		\
			curr->expand_id = max+1;								\
			curr = curr->next;										\
		}															\
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

#define TOKENIZE_VAR()												\
	{																\
		Parser *args = mini_tokenizer(value);						\
		Parser *curr = args;										\
		REATTRIBUTE_ID();											\
		if ((!ft_isspace(value[0]) || !ft_strlen(el->content))		\
			&& args && args->token != t_wildcard) {					\
			MERGE_FIRST_NODE();										\
		}															\
		INSERT_NEW_ARGS();											\
	}

char *ft_getenv_alloc(Env *env, char *target);
void	expand_parameter(Env *env, Command *cmd, Parser *el, int max);
void	expand_vars(Env *env, Parser *el, int max);
Parser	*expand_wildcard(Parser *el, int max);
void	arithmetic(Env *env, Parser *el);
void	tilde(Parser *el);
