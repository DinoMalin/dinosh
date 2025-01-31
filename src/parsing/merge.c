#include "parsing.h"

void merge_one_node(Parser *head) {
	Parser *merge = head->next;

	while (merge && merge->index == head->index) {
		Parser *next = merge->next;

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

