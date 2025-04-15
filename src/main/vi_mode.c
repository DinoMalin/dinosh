#include "minishell.h"
#include <fcntl.h>

int open_vim(int count, int key) {
	(void)count;
	(void)key;
	if (rl_editing_mode == 1)
		return 1;

	char *temp = get_random_file_name();
	int fd = open(temp, O_WRONLY | O_CREAT, 0644);
	if (!fd)
		return 0;

	write(fd, rl_line_buffer, ft_strlen(rl_line_buffer));
	close(fd);

	int pid = fork();
	if (pid < 0)
		return 0;
	else if (!pid) {
		char *args[] = {"/bin/vim", temp, NULL};
		execve("/bin/vim", args, NULL);
		return 0;
	}

	waitpid(pid, NULL, 0);
	fd = open(temp, O_RDONLY, 0644);
	char *new_line = get_next_line(fd);
	if (!new_line)
		return 0;

	int len = ft_strlen(new_line);
	rl_replace_line(new_line, 1);
	rl_point = len;
	rl_end = len;
	rl_done = 1;

	unlink(temp);
	return 0;
}
