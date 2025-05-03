#include "minishell.h"

#define SIZE 10
#define MILK_BUFFER()					\
	{									\
		res = clean_join(res, buff);	\
		index = 0;						\
		bzero(buff, SIZE);				\
	}

char *expand_exclamation_mark(char *input) {
	char *res = ft_strdup("");
	bool single_quotes = false;
	bool double_quotes = false;
	bool expanded = false;

	char buff[SIZE] = {};
	int index = 0;

	while (*input) {
		if (*input == '\\') {
			input++;
			continue;
		}

		if (!ft_strncmp(input, "!!", 2) && !single_quotes) {
			expanded = true;
			HIST_ENTRY *l = history_get(history_base);
			MILK_BUFFER();
			if (l)
				res = clean_join(res, l->line);
			else
				res = clean_join(res, "!!"); // should return an error
			input++;
		} else {
			buff[index] = *input;
			index++;
			if (index == SIZE) {
				MILK_BUFFER();
			}
			
		}

		if (*input == '\'' && !double_quotes)
			single_quotes = true;
		else if (*input == '"' && !single_quotes)
			double_quotes = true;
		else {
			single_quotes = false;
			double_quotes = false;
		}

		input++;
	}

	MILK_BUFFER();
	if (expanded)
		printf("%s\n", res);
	return res;
}
