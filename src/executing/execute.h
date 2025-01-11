#include "minishell.h"

#define IS_CHILD(x) (!x)
#define IS_BUILTIN(x) (x == ECHO || x == CD || x == PWD || x == EXPORT \
						|| x == UNSET|| x == ENV || x == ENV || x == EXIT)

#define TO_FLAGS O_WRONLY | O_CREAT | O_TRUNC
#define APPEND_FLAGS O_WRONLY | O_CREAT | O_APPEND
#define FROM_FLAGS O_RDONLY
#define HEREDOC_FILE "/tmp/dino_heredoc"

char	*find_path(char **env, char *cmd);
void	redirect(Command *cmd);
