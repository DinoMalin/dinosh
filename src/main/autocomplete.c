#include "minishell.h"

extern Env *environ;
char *builtins[] = {
	"echo",
	"cd",
	"pwd",
	"export",
	"unset",
	"env",
	"exit",
	"set",
	"type",
	"jobs",
	"fg",
	"bg",
	"hash",
	NULL
};

char *find_var(const char *text, int state) {
    static int len = 0;
	static Env *env = NULL;
	static bool is_extended = false;

    if (!state) {
		env = environ;
        len = ft_strlen(text);
		is_extended = *text == '{';
    }

	if (is_extended)
		text++;

    while (env) {
        if (!ft_strncmp(env->var, (char *)text, len)) {
			char *res = env->var;
			env = env->next;
            return ft_strdup(res);
        }
		env = env->next;
    }

    return NULL;
}

char *find_commands(const char *text, int state) {
	static int len, i;

	if (!state) {
		i = 0;
		len = ft_strlen(text);
	}

	char *cmd = builtins[i];
	while (builtins[i++]) {
		if (!ft_strncmp(cmd, (char *)text, len))
			return ft_strdup(cmd);
		cmd = builtins[i];
	}

	return NULL;
}

bool is_a_command(int start) {
	if (!start)
		return true;

	if (!ft_isspace(rl_line_buffer[start]))
		start--;
	while (start && ft_isspace(rl_line_buffer[start]))
		start--;

	if (rl_line_buffer[start] == '|'
	||	rl_line_buffer[start] == ';'
	||	rl_line_buffer[start] == '&'
	)
		return true;

	return false;
}

char **completion(const char *text, int start, int end) {
	(void)start;
	(void)end;
	rl_attempted_completion_over = 1;

	if ((start > 0 && rl_line_buffer[start-1] == '$')
		|| (start > 1 && rl_line_buffer[start-2] == '$'
		&& rl_line_buffer[start-1] == '{')) {
		return rl_completion_matches(text, find_var);
	}
	if (is_a_command(start)) {
		return rl_completion_matches(text, find_commands);
	}

	return rl_completion_matches(text, rl_filename_completion_function);
}
