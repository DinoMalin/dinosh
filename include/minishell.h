#include "libft.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <readline/readline.h>

#define ft_isspace(x) ((x >= '\t' && x <= '\r') || x == ' ')
#define can_expand(x) (x->token == t_double_quotes || x->token == t_word)
#define is_redir(x) (x == t_append || x == t_heredoc || x == t_to || x == t_from)
#define xfree(x) if (x) {free(x);}
#define IS_CHILD(x) (!x)
#define IS_BUILTIN(x) (x == ECHO || x == CD || x == PWD || x == EXPORT \
						|| x == UNSET|| x == ENV || x == ENV)

#define TO_FLAGS O_WRONLY | O_CREAT | O_TRUNC
#define APPEND_FLAGS O_WRONLY | O_CREAT | O_APPEND
#define FROM_FLAGS O_RDONLY
#define HEREDOC_FILE "/tmp/dino_heredoc"

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
		if (!ft_strncmp(str, start, len)) {	\
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

#define ADD_COMMAND(head, new, last)		\
	{										\
		if (!head)							\
			head = new;						\
		else								\
			last->next = new;				\
	}										\

typedef enum {
	t_word,
	t_to,
	t_append,
	t_from,
	t_heredoc,
	t_single_quotes,
	t_double_quotes,
	t_subshell,
	t_pipe,
} Token;

typedef enum {
	no_error,
	unclosed_token,
	empty_redir,
	redir_toward_redir
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
} Transmission;

/* === Executing linked list ===*/
typedef struct Command {
	char			*cmd;
	char			**av;
	int				ac;
	t_redir			*redirs;
	Error			error;
	CommandType		type;
	Transmission	transmission;
	struct Command	*next;
} Command;

typedef struct {
	char *prompt;
	char **env;
} Prompt;

/* ====== PARSING ====== */
Node	*tokenize(char *str);
char	*expand(char *str, char **envp);
Node	*parse(char *str, char **envp);

/* ====== PROCESSING ====== */
void	 init_ac(Command *head);
void	 check_redir(Command *cmd, Node **data);
void	 check_type(Command *cmd, Token token);
Command	*process(Node *data);

/* ====== EXECUTING ====== */
char	*find_path(char **env, char *cmd);
void	redirect(Command *cmd);
void	execute(Command *cmd, Prompt *prompt);

/* ====== BUILTINS ====== */
char	*ft_getenv(char **envp, char *target);
char	**copy_env(char **env);
char	**modify_env(char **env, char *var, char *content);
void	delete_var(char **env, char *var);
void	builtin(Command *cmd, Prompt *prompt);
void	echo(Command *cmd);
void	cd(Command *cmd, char **env);
void	pwd(Command *cmd);
void	export(Command *cmd, Prompt *prompt);
void	unset(Command *cmd, Prompt *prompt);

/* ====== MEMORY ====== */
void	free_av(char **av);
void	free_cmds(Command *list);
void	free_node(Node *node);
void	free_list(Node *list);

/* ====== UTILS ====== */
char	*clean_join(char *origin, const char *to_join);
char	**clean_strsjoin(char **origin, char *to_join);
t_redir	*clean_redirjoin(t_redir *origin, t_redir to_join);
int		len_until_chr(char *str, char c);
void	handle_prompt(Prompt *prompt);

/* ====== ERROR ====== */
bool	parsing_error(Node *head);
bool	process_error(Command *head);
bool	has_parsing_errors(Node *head);

/* ====== TESTS ====== */
void	tests_parsing(char **envp);

