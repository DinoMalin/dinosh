#include "parse.h"

static bool its_a_match(char *str, char *entry) {
	int i = 0;
	int j = 0;

	while (str[i] && entry[j]) {
		if (str[i] == '*') {
			while (str[i] == '*') {
				i++;
			}
			while (entry[j] && entry[j] != str[i]) {
				j++;
			}
			if (!str[i])
				return true;
		}
		while (EQUAL(str[i], entry[j])) {
			i++;
			j++;
		}
		if (NOT_EQUAL(str[i], entry[j]))
			return false;
	}
	while (str[i] == '*') {
		i++;
	}
	if (str[i])
		return false;
	return true;
}

char *get_entries(char *str) {
	DIR *dir = opendir(".");
	struct dirent *entry = NULL;
	char *res = ft_strdup("");

	if (!dir) {
		perror("dinosh: opendir");
		return NULL;
	}

	while ((entry = readdir(dir))) {
		if (!ft_strcmp(entry->d_name, ".") || !ft_strcmp(entry->d_name, ".."))
			continue;
		if (its_a_match(str, entry->d_name)) {
			if (str[0] != '.' && entry->d_name[0] == '.')
				continue;
			res = clean_join(res, entry->d_name);
			res = clean_join(res, " ");
		}
	}

	closedir(dir);
	return res;
}

char *expand_wildcard(char *str) {
	char *entries = get_entries(str);
	char *res = ft_strtrim(entries, " ");
	free(entries);
	return res;
}

