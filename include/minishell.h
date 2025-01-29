#include "libft.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

#define xfree(x) if (x) {free(x);}

#define UPDATE_CODE_VAR(code)						\
	{												\
		char *s = ft_itoa(code);					\
		modify_env(&ctx.env, "?", s);				\
		set_special(ctx.env, "?", true);			\
		free(s);									\
	}

typedef enum {
	t_word,
	t_to,
	t_append,
	t_from,
	t_heredoc,
	t_single_quotes,
	t_double_quotes,
	t_subshell,
	t_and,
	t_or,
	t_pipe,
} Token;

typedef enum {
	no_error,
	unclosed_token,
	empty_redir,
	redir_toward_redir,
	start_pipe
} Error;

/* === Parsing linked list ===*/
typedef struct Parser {
	char		*content;
	Token		token;
	struct Parser	*next;
	Error		error;
	int			index;	// isn't used to count : the index will sometime be
} Parser;					// non-consecutives. It's used to know if two args
						// must be merged

typedef enum {
	r_to,
	r_append,
	r_from,
	r_heredoc,
} Redir;

typedef enum {
	BASIC,
	SUBSHELL,
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

typedef enum {
	ANY,
	PIPE,
	AND,
	OR,
} Transmission;

/* === Executing linked list ===*/
typedef struct Command {
	char			*cmd;
	Parser			*args;
	char			**av;
	int				ac;
	t_redir			*redirs;
	Error			error;
	CommandType		type;
	Transmission	from;
	Transmission	to;
	int				exit_code; // used for builtins
	pid_t			pid;
	struct Command	*next;
} Command;

typedef struct Env {
	char		*var;
	char		*value;
	bool		special;
	bool		intern;
	struct Env	*next;
} Env;

typedef struct {
	char	*input;
	Env		*env;
	bool	exit;
	int		code;
} Context;

/* ====== MINISHELL ====== */
Parser	*tokenize(char *str);
Command	*process(Parser *data);
void	expand(Command *cmd, Env *env);
void	execute(Command *cmd, Context *ctx);

/* ====== SIGNALS ====== */
void	sig_handler(int sig);
int		rl_hook();

/* ====== BUILTINS ====== */
Env		*create_env(char **env);
void	modify_env(Env **env, char *target, char *new_value);
char	*ft_getenv(Env *env, char *target);
char	**get_envp(Env *env);
void	set_special(Env *env, char *target, bool special);
void	set_intern(Env *env, char *target, bool intern);
void	builtin(Command *cmd, Context *ctx);

/* ====== MEMORY ====== */
void	free_av(char **av);
void	free_cmds(Command *list);
void	free_node(Parser *node);
void	free_list(Parser *list);
void	free_env(Env *env);

/* ====== UTILS ====== */
char	*clean_join(char *origin, const char *to_join);
char	**clean_strsjoin(char **origin, char *to_join);
t_redir	*clean_redirjoin(t_redir *origin, t_redir to_join);
int		len_until_chr(char *str, char c);
void	handle_input(Context *ctx);

/* ====== ERROR ====== */
bool	parsing_error(Parser *head);
bool	process_error(Command *head);
bool	has_parsing_errors(Parser *head);

/* ====== TESTS ====== */
void	tests_parsing(char **envp);

