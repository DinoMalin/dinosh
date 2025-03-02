#include "minishell.h"
#include <dirent.h>

#define CAN_EXPAND(x) (((x->quoting == doubles || x->token == t_word) && ft_strchr(x->content, '$') && x->expand_id != -1) || x->token == t_var) // yeah maybe its not right anymore

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


char	*ft_getenv_alloc(Env *env, char *target);
void	expand_parameter(Env *env, Command *cmd, Parser *el, int max);
void	expand_vars(Env *env, Parser *el, int max);
Parser	*expand_wildcard(Parser *el, int max);
void	arithmetic(Env *env, Parser *el);
void	tilde(Parser *el);
void	control_substitution(Context *ctx, Command *cmd, Parser *el, int max);
