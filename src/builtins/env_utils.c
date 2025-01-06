#include "minishell.h"

char **copy_env(char **env) {
	int size = 0;
	while (env[size]) {
		size++;
	}
	char **res = malloc((size + 1) * sizeof(char *));
	int i = 0;
	while (env[i]) {
		res[i] = ft_strdup(env[i]);
		i++;
	}
	res[i] = NULL;
	return res;
}

static char **ft_getenv_ptr(char **envp, char *target) {
	int target_len = ft_strlen(target);

	for (int i = 0; envp[i]; i++) {
		int key_len = len_until_chr(envp[i], '=');
		if (!ft_strncmp(envp[i], target, key_len) && key_len == target_len)
			return envp + i;
	}
	return NULL;
}

char **modify_env(char **env, char *var, char *content) {
	char *new_var = ft_strdup(var);
	new_var = clean_join(new_var, "=");
	new_var = clean_join(new_var, content);

	char **var_in_env = ft_getenv_ptr(env, var);
	if (!var_in_env)
		return clean_strsjoin(env, new_var);

	free(*var_in_env);
	*var_in_env = new_var;
	return env;
}
