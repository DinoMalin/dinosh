#include "expand.h"

char **get_entries(Parser *head) {
	DIR *dir = opendir(".");
	struct dirent *entry = NULL;
	char **res = ft_calloc(1, sizeof(char*));

	while (head && !head->content[0]) {
		head = head->next;
	}

	if (!dir) {
		perror("dinosh: opendir");
		return NULL;
	}

	while ((entry = readdir(dir))) {
		if (its_a_match(entry->d_name, head->content)) {
			if (head && head->content[0] != '.' && entry->d_name[0] == '.')
				continue;
			res = clean_strsjoin(res, ft_strdup(entry->d_name));
		}
	}

	closedir(dir);
	return res;
}

void sort_entries(char **entries) {
	for (int i = 0; entries[i]; i++) {
		for (int j = i + 1; entries[j]; j++) {
			if (ft_strcmp(entries[i], entries[j]) > 0) {
				char *tmp = entries[i];
				entries[i] = entries[j];
				entries[j] = tmp;
			}
		}
	}
}

Parser *expand_globing(Parser *el, int max) {
	char **entries = get_entries(el);
	sort_entries(entries);

	if (entries[0]) {
		free(el->content);
		el->content = entries[0];
		el->expand_id = max + 1;

		Parser *next = el->next;
		for (int i = 1; entries[i]; i++) {
			Parser *new = ft_calloc(1, sizeof(Parser));

			new->content = entries[i];
			new->id = max + i + 1;
			new->expand_id = max + 1;

			el->next = new;
			el = el->next;
		}

		el->next = next;
	}

	free(entries);
	return el;
}

bool this_node_has_globing(Parser *node) {
	char tools[] = {'*', '[', '?', '\0'};
	for (int i = 0; tools[i]; i++) {
		if (ft_strchr(node->content, tools[i]))
			return true;
	}
	return false;
}

void globing(Parser *el) {
	Parser *curr = el;


	while (curr) {
		if (curr->glob)
			curr = expand_globing(curr, max_id(el));
		curr = curr->next;
	}
}
