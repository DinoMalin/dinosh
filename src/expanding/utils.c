#include "expand.h"

static Parser *lstlast(Parser *el) {
	while (el->next) {
		el = el->next;
	}
	return el;
}

void add_tokenized_args(Parser *el, char *value, int max) {
	Parser *args = mini_tokenizer(value);
	if (!args) {
		free(el->content);
		el->content = ft_strdup("");
		return;
	}

	Parser *next = args->next;
	free(el->content);
	el->content = ft_strdup(args->content);
	el->expand_id = max+1;
	free_node(args);
	args = next;

	Parser *curr = args;
	for (int i = 0; curr; i++) {
		curr->id = max+i+(ft_strlen(curr->content) > 0);
		curr->expand_id = max+1;
		curr = curr->next;
	}

	next = el->next;
	el->next = args;
	lstlast(el)->next = next;
}
