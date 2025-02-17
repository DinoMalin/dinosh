#include "minishell.h"

bool is_number(char *str) {
	if (!*str)
		return false;

	if (*str == '+' || *str == '-' || ft_isdigit(*str))
		str++;
	else
		return false;

	while (*str) {
		if (!ft_isdigit(*str))
			return false;
		str++;
	}

	return true;
}

bool var_is_valid(char *name) {
	int i = 0;
	for (; name[i]; i++) {
		if (!(name[i] >= 'a' && name[i] <= 'z')
		 && !(name[i] >= 'A' && name[i] <= 'Z')
		 && !(name[i] >= '0' && name[i] <= '9' && i != 0)
		 && !(name[i] == '_')) {
			return false;
		}
	}
	if (!i)
		return false;
	return true;
}

char *get_random_file_name() {
	int fd = open("/dev/random", O_RDONLY);
	if (fd < 0)
		return NULL;

	char buff[21];
	read(fd, &buff, 20);
	buff[20] = '\0';
	for (int i = 0; i < 20; i++) {
		if (!ft_isprint(buff[i])) {
			buff[i] *= buff[i] < 0 ? -1 : 1;
			buff[i] = (buff[i] % 94) + 32;
		}
	}

	return ft_strjoin("/tmp/", buff);
}
