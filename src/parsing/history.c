#include "minishell.h"

#define SIZE 10
#define MILK_BUFFER()					\
	{									\
		res = clean_join(res, buff);	\
		index = 0;						\
		bzero(buff, SIZE);				\
	}

#define CHECK_QUOTING()								\
	{												\
		if (*input == '\'' && !double_quotes)		\
			single_quotes = true;					\
		else if (*input == '"' && !single_quotes)	\
			double_quotes = true;					\
		else {										\
			single_quotes = false;					\
			double_quotes = false;					\
		}											\
	}

#define ADD_TO_BUFFER()					\
	{									\
			buff[index] = *input;		\
			index++;					\
			if (index == SIZE) {		\
				MILK_BUFFER();			\
			}							\
	}

#define GO_TO_WHITESPACE()							\
	{												\
		while (*input && !ft_isspace(*input)) {		\
			input++;								\
		}											\
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

		if (!single_quotes && *input == '!') {
			char *tools = (input+1);
			if (*tools == '!') {
				expanded = true;
				HIST_ENTRY *l = history_get(history_base+history_length-1);
				MILK_BUFFER();
				if (l)
					res = clean_join(res, l->line);
				else
					res = clean_join(res, "!!"); // should return an error
				input++;
			} else if (ft_isdigit(*tools)) {
				int offset = ft_atoi(tools);
				expanded = true;
				HIST_ENTRY *l = history_get(history_base+offset-1);
				MILK_BUFFER();
				if (l)
					res = clean_join(res, l->line);
				else
					res = clean_join(res, "!!"); // should return an error

				input++;
				while (*input && ft_isdigit(*(input+1))) {
					input++;
				}
			} else if (*tools == '-' && ft_isdigit(*(tools+1))) {
				int offset = ft_atoi(tools+1);
				expanded = true;
				HIST_ENTRY *l = history_get(history_base+history_length-1-offset);
				MILK_BUFFER();
				if (l)
					res = clean_join(res, l->line);
				else
					res = clean_join(res, "!!"); // should return an error

				input += 2;
				while (*input && ft_isdigit(*(input+1))) {
					input++;
				}
			} else {
				ADD_TO_BUFFER();
				input++;
			}
		} else {
			ADD_TO_BUFFER();
		}

		CHECK_QUOTING();
		input++;
	}

	MILK_BUFFER();
	if (expanded)
		printf("%s\n", res);
	return res;
}
