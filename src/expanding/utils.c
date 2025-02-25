#include "expand.h"

static Parser *lstlast(Parser *el) {
	while (el->next) {
		el = el->next;
	}
	return el;
}

void add_tokenized_args(Parser *el, char *value, int max) {
		Parser *args = mini_tokenizer(value);
		Parser *curr = args;

		for (int i = 0; curr; i++) {
			curr->id = max+i+(ft_strlen(curr->content) > 0);
			curr->expand_id = max+1;
			curr = curr->next;
		}

		if ((!ft_isspace(value[0]) || !ft_strlen(el->content))
			&& args && args->token != t_wildcard
		) {
			Parser *next = args->next;
			el->content = clean_join(el->content, args->content);
			free_node(args);
			args = next;
		}

		Parser *next = el->next;
		el->next = args;
		lstlast(el)->next = next;
}
