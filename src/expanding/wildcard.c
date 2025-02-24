#include "expand.h"

static bool its_a_match(Parser *head, char *str) {
	int i = 0;
	Parser *curr = head;

	while (SAME_ID(curr, head) && *str) {
		if (curr->token == t_wildcard) {
			while (IS_WILDCARD(curr, head)) {
				curr = curr->next;
			}
			if (!SAME_ID(curr, head))
				return true;
			while (SAME_ID(curr, head) && *str && curr->content[i] != *str) {
				str++;
			}

			char *next_try = str;
            while (*next_try) {
                if (its_a_match(curr, next_try))
                    return true;
                next_try++;
            }
            return false;
		}

		if (curr->content[i] != *str)
			return false;
		str++;
		INCREMENT_CONTENT(curr, i);
	}

	while (SAME_ID(curr, head) && curr->token == t_wildcard) {
		curr = curr->next;
	}

	return !SAME_ID(curr, head) && !*str;
}

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
		if (its_a_match(head, entry->d_name)) {
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

Parser *expand_wildcard(Parser *el, int max) {
	char **entries = get_entries(el);
	sort_entries(entries);

	if (entries[0]) {
		free(el->content);
		el->content = entries[0];
		el->expand_id = max + 1;

		// erase the args with the same id
		Parser *del = el->next;
		while (SAME_ID(del, el)) {
			Parser *next = del->next;
			free_node(del);
			del = next;
		}
		el->next = del;

		// place the new args
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

