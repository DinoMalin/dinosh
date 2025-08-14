#include "minishell.h"
#include <dirent.h>
#include <stdbool.h>
#include <ctype.h>

#define ERROR_VALUE 9223372036854775809UL
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

typedef struct s_arit {
	char *value;
	char *baseVal;
	bool isEnv;
	bool isOp;
	struct s_arit *next;
	struct s_arit *prev;
} t_arit;

// Main functions
char	*ft_getenv_alloc(Env *env, char *target);
void	parameter(Env *env, Command *cmd, Parser *el, int max);
void	var(Env *env, Parser *el, int max);
void	arithmetic(Env *env, Parser *el);
void	tilde(Parser *el);
void	control_substitution(Context *ctx, Command *cmd, Parser *el, int max);
void	process_substitution(Context *ctx, Command *cmd, Parser *el);
bool	its_a_match(char *str, char *pattern);
int		max_id(Parser *head);

// Arithmetic utility functions
long long	get_val(t_arit *var, Env *env);
long long	cmp_val(t_arit *var1, t_arit *var2, Env *env, int (*cmp)(long long, long long));
int		equal_cmp(long long a, long long b);
int		not_equal_cmp(long long a, long long b);
int		less_than_cmp(long long a, long long b);
int		greater_than_cmp(long long a, long long b);
int		less_equal_cmp(long long a, long long b);
int		greater_equal_cmp(long long a, long long b);
void	free_arit(t_arit *content);

// Arithmetic parsing functions
bool	is_only_digit(char *str);
bool	is_op(char *str);
char	*var_in_env(Env *env, char *str);
char	*val_in_env(Env *env, char *str);
char	*assign_op(char *str);
char	**tokenize_arithmetic(char *str);
int		find_matching_paren(char **tokens, int start);
long long	evaluate_parentheses(char **tokens, int start, int end, Env *env, char *fullStr);
t_arit	*pre_parse(char *str, Env *env);

// Arithmetic operation functions
long long	do_assign(t_arit *var1, t_arit *var2, Env *env, char *fullStr);
long long	do_add(t_arit *var1, t_arit *var2, Env *env);
long long	do_minus(t_arit *var1, t_arit *var2, Env *env);
long long	do_div(t_arit *var1, t_arit *var2, Env *env, char *fullStr);
long long	do_mod(t_arit *var1, t_arit *var2, Env *env, char *fullStr);
long long	do_mul(t_arit *var1, t_arit *var2, Env *env);
long long	do_equal(t_arit *var1, t_arit *var2, Env *env);
long long	do_not_equal(t_arit *var1, t_arit *var2, Env *env);
long long	do_less_than(t_arit *var1, t_arit *var2, Env *env);
long long	do_greater_than(t_arit *var1, t_arit *var2, Env *env);
long long	do_less_equal(t_arit *var1, t_arit *var2, Env *env);
long long	do_greater_equal(t_arit *var1, t_arit *var2, Env *env);
long long	do_logical_and(t_arit *var1, t_arit *var2, Env *env);
long long	do_logical_or(t_arit *var1, t_arit *var2, Env *env);
int		get_precedence(char *op);
long long	evaluate_operation(char *op, t_arit *var1, t_arit *var2, Env *env, char *fullStr);
long long	do_op(t_arit *tokens, Env *env, char *fullStr);
