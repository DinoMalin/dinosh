#include "libft.h"
#include <stdbool.h>
#include <stdio.h>

#define isspace(x) ((x >= '\t' && x <= '\r') || x == ' ')
#define end_arg(x) (isspace(x) || x == '|' || x == '<' || x == '>' || x == '\'' || x == '"')
#define can_expand(x) (x->token == double_quotes || x->token == arg)
#define is_operator(x) (x == t_pipe || x == append || x == heredoc \
					|| x == to || x == from)
#define is_redir(x) (x == append || x == heredoc || x == to || x == from)
#define OPERATOR(x) x == t_pipe ?	ft_strdup("|") : \
					x == append ?	ft_strdup(">>") : \
					x == heredoc ?	ft_strdup("<<") : \
					x == to ?		ft_strdup(">") : \
									ft_strdup("<")
#define xfree(x) if (x) {free(x);}
#define IS_CHILD(x) (!x)
#define IS_BUILTIN(x) (x == ECHO || x == CD || x == PWD || x == EXPORT \
						|| x == UNSET|| x == ENV || x == ENV)

#define add_command(head, new, last)		\
	{										\
		if (!head)							\
			head = new;						\
		else								\
			last->next = new;				\
	}										\

typedef enum {
	arg,
	to,
	append,
	from,
	heredoc,
	single_quotes,
	double_quotes,
	t_pipe,
} Token;

typedef enum {
	single_quote,
	double_quote,
	empty_redir,
	redir_toward_redir
} Error_Type;

typedef struct {
	int			index;
	Error_Type	type;
} Error;

/* === Parsing linked list ===*/
typedef struct Node {
	char		*content;
	Token		token;
	struct Node	*next;
	Error		error;
	int			index;	// isn't used to count : the index will sometime be
} Node;					// non-consecutives. It's used to know if two args
						// must be merged

typedef enum {
	r_to,
	r_append,
	r_from,
	r_heredoc,
} Redir;

typedef enum {
	BASIC,
	ECHO,
	CD,
	PWD,
	EXPORT,
	UNSET,
	ENV,
	EXIT
} CommandType;

typedef struct {
	char	*file;
	Redir	type;
} t_redir;

/* === Executing linked list ===*/
typedef struct Command {
	char			*cmd;
	char			**av;
	int				ac;
	t_redir			*redirs;
	Error			error;
	CommandType		type;
	struct Command	*next;
} Command;

/* ====== PARSING ====== */
Node	*tokenize(char *str);
char	*expand(char *str, char **envp);
Node	*parse(char *str, char **envp);

/* ====== PROCESSING ====== */
void init_ac(Command *head);
void check_redir(Command *cmd, Node **data);
void check_type(Command *cmd);
Command	*process(Node *data);

/* ====== EXECUTING ====== */
char *find_path(char **env, char *cmd);
void execute(Command *cmd, char **env);

/* ====== BUILTINS ====== */
char **copy_env(char **env);
char **modify_env(char **env, char *var, char *content);
void delete_var(char **env, char *var);

/* ====== MEMORY ====== */
void	free_av(char **av);
void	free_cmds(Command *list);
void	free_node(Node *node);
void	free_list(Node *list);

/* ====== UTILS ====== */
char	*clean_join(char *origin, const char *to_join);
char	**clean_strsjoin(char **origin, char *to_join);
t_redir *clean_redirjoin(t_redir *origin, t_redir to_join);
int len_until_chr(char *str, char c);

/* ====== TESTS ====== */
void	tests_parsing(char **envp);

