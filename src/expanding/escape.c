#include "expand.h"

bool	this_id_has_wildcard(Parser *head) {
	int id = head->id;
	while (head && head->id == id) {
		if (head->token == t_wildcard && !head->escaped)
			return true;
		head = head->next;
	}

	return false;
}

void reescape(Parser *head) {
	while (head) {
		if (this_id_has_wildcard(head)) {
			if ((head->token == t_wildcard || head->token == t_backslash)
				&& head->escaped) {
				char *tmp = head->content;
				head->content = ft_strjoin("\\", head->content);
				free(tmp);
			}
		}
		head = head->next;
	}
}
