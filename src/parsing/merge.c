#include "parsing.h"

void merge_one_node(Parser *head) {
	Parser *merge = head->next;

	while (merge && merge->id == head->id) {
		Parser *next = merge->next;

		if (merge->glob)
			head->glob = true;

		head->content = clean_join(head->content, merge->content);
		head->next = next;
		free_node(merge);

		merge = next;
	}
}

void merge(Parser *head) {
	while (head) {
		merge_one_node(head);
		head = head->next;
	}
}

