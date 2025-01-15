#include "parse.h"

void merge(Node *head) {
	Node *next;
	Node *after_next;

	while (head && head->next) {
		next = head->next;
		while (next && head->index == next->index) {
			head->content = clean_join(head->content, next->content);
			after_next = next->next;
			head->next = after_next;
			free_node(next);
			next = after_next;
		}
		head = head->next;
	}
}

Node *parse(char *str, char **envp) {
	Node *res_head = tokenize(str);
	Node *curr = res_head;

	if (has_parsing_errors(res_head)) {
		return res_head;
	}

	while (curr) {
		if (CAN_EXPAND(curr)) {
			char *expanded = expand(curr->content, envp);
			free(curr->content);
			curr->content = expanded;
		}
		if (CAN_WILDCARD(curr)) {
			char *expanded = expand_wildcard(curr->content);
			free(curr->content);
			curr->content = expanded;
		}

		for (int i = 0; curr->content[i]; i++) {
			if (IS_STAR(curr->content[i]))
				curr->content[i] = '*';
		}

		curr = curr->next;
	}

	if (res_head)
		merge(res_head);
	return res_head;
}
