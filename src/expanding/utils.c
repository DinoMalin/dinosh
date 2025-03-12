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

int	ft_atoi_base(char *str)
{
	int	result = 0;
	int	base = 10;
	int	i = 0;
	int	sign = 1;

	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}

	if (str[i] == '0')
	{
		if (str[i + 1] == 'x' || str[i + 1] == 'X')
		{
			base = 16;
			i += 2;
		}
		else if (str[i + 1] == 'b' || str[i + 1] == 'B')
		{
			base = 2;
			i += 2;
		}
		else
			base = 8;
	}

	while (str[i])
	{
		char c = str[i];
		int value = -1;
		if (c >= '0' && c <= '9')
			value = c - '0';
		else if (c >= 'A' && c <= 'F')
			value = c - 'A' + 10;
		else if (c >= 'a' && c <= 'f')
			value = c - 'a' + 10;
		if (value == -1 || value >= base)
			break;
		result = result * base + value;
		i++;
	}
	return result * sign;
}
