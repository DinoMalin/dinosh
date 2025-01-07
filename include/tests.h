#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define STRINGS_DIFFER(x, y)	((!x && y) || (x && !y) \
								|| (x && y && ft_strcmp(x, y)))

#define FAIL(log)					\
{									\
	ft_fprintf(2, log "\n");		\
	return 0;						\
}

#define AV(...) (char*[]){__VA_ARGS__, NULL}
#define NO_AV (char*[]){NULL}

#define REDIR(file, type) (t_redir){file, type}
#define NO_REDIR (t_redir[]){{NULL, 0}}
#define REDIRS(...) (t_redir[]){__VA_ARGS__, {NULL, 0}}

#define COMMAND(cmd, av, redirs, type) \
	(Command){cmd, av, redirs, {0,0}, type, NULL}
