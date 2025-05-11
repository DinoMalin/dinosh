#include "minishell.h"

#define SIZE 10
#define MILK_BUFFER()					\
	{									\
		res = clean_join(res, buff);	\
		index = 0;						\
		bzero(buff, SIZE);				\
	}

#define CHECK_QUOTING()											\
	{															\
		if (*input == '\'' && !double_quotes && !escape)		\
			single_quotes = true;								\
		else if (*input == '"' && !single_quotes && !escape)	\
			double_quotes = true;								\
		else {													\
			single_quotes = false;								\
			double_quotes = false;								\
		}														\
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

#define NO_RESULT()			\
	{						\
		free(res);			\
		return NULL;		\
	}

char *search_history(char *prefix) {
	int len = ft_strlen(prefix);
	for (int i = history_length; i >= history_base; i--) {
		HIST_ENTRY *e = history_get(i);
		if (e && !ft_strncmp(prefix, e->line, len)) {
			return e->line;
		}
	}
	return NULL;
}

char *expand_exclamation_mark(char *input) {
	char *res = ft_strdup("");
	bool single_quotes = false;
	bool double_quotes = false;
	bool escape = false;
	bool expanded = false;

	char buff[SIZE] = {};
	int index = 0;

	while (*input) {
		if (escape)
			escape = false;
		if (*input == '\\') {
			ADD_TO_BUFFER();
			escape = true;
			input++;
		}

		if (!single_quotes && *input == '!' && !escape) {
			char *tools = (input+1);
			if (*tools == '!') {
				expanded = true;
				HIST_ENTRY *l = history_get(history_base+history_length-1);
				MILK_BUFFER();
				if (l)
					res = clean_join(res, l->line);
				else {
					NO_RESULT();
				}

				input++;
			} else if (ft_isdigit(*tools)) {
				int offset = ft_atoi(tools);
				expanded = true;
				HIST_ENTRY *l = history_get(history_base+offset-1);
				MILK_BUFFER();
				if (l)
					res = clean_join(res, l->line);
				else {
					NO_RESULT();
				}

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
				else {
					NO_RESULT();
				}

				input += 2;
				while (*input && ft_isdigit(*(input+1))) {
					input++;
				}
			} else {
				MILK_BUFFER();
				int end = 0;
				char *str = (input+1);
				while (str[end] && !ft_isspace(str[end]))
					end++;

				if (!end) {
					ADD_TO_BUFFER();
					MILK_BUFFER();
				} else {
					char *sub = ft_substr(str, 0, end);
					char *line = search_history(sub);
					free(sub);
					if (!line) {
						NO_RESULT();
					}

					res = clean_join(res, line);
					input += end;
					expanded = true;
				}
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
