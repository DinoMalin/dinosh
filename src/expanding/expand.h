#include "minishell.h"
#include <dirent.h>

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

char	*ft_getenv_alloc(Env *env, char *target);
void	parameter(Env *env, Command *cmd, Parser *el, int max);
void	var(Env *env, Parser *el, int max);
void	arithmetic(Env *env, Parser *el);
void	tilde(Parser *el);
void	control_substitution(Context *ctx, Command *cmd, Parser *el, int max);
void	process_substitution(Context *ctx, Command *cmd, Parser *el);
Parser	*expand_wildcard(Parser *el, int max);
