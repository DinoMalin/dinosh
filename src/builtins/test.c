#include "builtins.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ARG(n) reverse ? n+1 : n

void test(Command *cmd, Context *ctx) {
	(void)ctx;
	if (cmd->ac < 3)
		BUILTIN_ERROR("test: not enough args");

	bool reverse = cmd->ac > 2 && !ft_strcmp(cmd->av[1], "!");
	bool resolved = false;

	// Strings tests
	if (cmd->ac == ARG(3) && !ft_strcmp(cmd->av[ARG(1)], "-z")) {
		cmd->exit_code = ft_strlen(cmd->av[ARG(2)]) > 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "=")) {
		cmd->exit_code = !!ft_strcmp(cmd->av[ARG(1)], cmd->av[ARG(3)]);
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "!=")) {
		cmd->exit_code = !ft_strcmp(cmd->av[ARG(1)], cmd->av[ARG(3)]);
		resolved = true;
	}

	// Numbers tests
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-eq")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("dinosh: numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) == ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-ne")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("dinosh: numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) != ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-ge")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("dinosh: numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) >= ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-gt")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("dinosh: numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) > ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-le")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("dinosh: numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) <= ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}
	else if (cmd->ac == ARG(4) && !ft_strcmp(cmd->av[ARG(2)], "-lt")) {
		if (!is_number(cmd->av[ARG(1)]) || !is_number(cmd->av[ARG(3)]))
			BUILTIN_ERROR("dinosh: numeric argument required");
		cmd->exit_code = !(ft_atoi(cmd->av[ARG(1)]) < ft_atoi(cmd->av[ARG(3)]));
		resolved = true;
	}

	// Files tests
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-b")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !S_ISBLK(buff.st_mode))
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-c")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !S_ISCHR(buff.st_mode))
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-d")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !S_ISDIR(buff.st_mode))
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-e")) {
		if (access(cmd->av[ARG(2)], F_OK) < 0)
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-f")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !S_ISREG(buff.st_mode))
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-g")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !(buff.st_mode & S_ISGID))
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-L")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !S_ISLNK(buff.st_mode))
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-p")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !S_ISFIFO(buff.st_mode))
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-r")) {
		if (access(cmd->av[ARG(2)], R_OK) < 0)
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-s")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !buff.st_size)
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-S")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !S_ISSOCK(buff.st_mode))
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-u")) {
		struct stat buff = {};
		if (lstat(cmd->av[ARG(2)], &buff) < 0 || !(buff.st_mode & S_ISUID))
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-w")) {
		if (access(cmd->av[ARG(2)], W_OK) < 0)
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}
	else if (cmd->ac == ARG(2) && !ft_strcmp(cmd->av[ARG(1)], "-x")) {
		if (access(cmd->av[ARG(2)], X_OK) < 0)
			cmd->exit_code = 1;
		else
			cmd->exit_code = 0;
		resolved = true;
	}

	if (resolved && reverse)
		cmd->exit_code = !cmd->exit_code;

	if (!resolved) {
		BUILTIN_ERROR("condition expected");
	}
}
