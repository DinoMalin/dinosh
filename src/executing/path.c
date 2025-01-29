#include "execute.h"

char *find_path(Env *env, char *cmd) {
	if (access(cmd, X_OK) == 0)
		return ft_strdup(cmd);

	char *path = ft_getenv(env, "PATH");
	if (!path)
		return NULL;

	char **paths = ft_split(path, ':');
	for (int i = 0; paths[i]; i++) {
		char *this_one_path = ft_strjoin(paths[i], "/");
		this_one_path = clean_join(this_one_path, cmd);

		if (access(this_one_path, X_OK) == 0) {
			free_av(paths);
			return this_one_path;
		}
		free(this_one_path);
	}
	free_av(paths);

	dprintf(2, "dinosh: %s: command not found\n", cmd);
	return NULL;
}
