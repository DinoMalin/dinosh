#include "parsing.h"

bool skip_whitespace(char **str) {
	bool res = false;
	while (ft_isspace(**str)) {
		(*str)++;
		res = true;
	}
	return res;
}

char *until(char **str, char *sep) {
	char *end = ft_strnstr(*str, sep, strlen(*str));
	if (!end)
		return NULL;

	char *res = ft_substr(*str, 0, end - *str);
	*str = end + ft_strlen(sep);

	return res;
}

char *operator(char **str, char *op) {
	*str += ft_strlen(op);
	return (ft_strdup(op));
}

char *parse_word(char **str, char *word_end) {
	char *end = *str;
	while (*end && !ft_strchr(word_end, *end)) {
		end++;
	}
	char *res = ft_substr(*str, 0, end - *str);
	*str = end;
	if (!ft_strlen(res)) {
		free(res);
		return NULL;
	}
	return res;
}

Parser *mini_tokenizer(char *str) {
	int id = 0;
	Parser *curr = NULL;
	Parser *head = NULL;

	while (*str) {
		if (skip_whitespace(&str))
			id++;
		PARSE_CHARACTER("*", t_wildcard);
		PARSE_WORD(MINI_WORD_END);
	}

	return head;
}

Parser *tokenize(char *str) {
	int id = 0;
	Parser *curr = NULL;
	Parser *head = NULL;

	while (*str) {
		if (skip_whitespace(&str))
			id++;
		PARSE_TOKEN("\"", "\"", t_double_quotes);
		PARSE_TOKEN("'", "'", t_single_quotes);
		PARSE_TOKEN("(", ")", t_subshell);
		PARSE_TOKEN("${", "}", t_var);
		PARSE_OPERATOR(";", t_semicolon);
		PARSE_OPERATOR(">&", t_missing_parameter);
		PARSE_OPERATOR("<&", t_missing_parameter);
		PARSE_OPERATOR(">>", t_append);
		PARSE_OPERATOR("<<", t_heredoc);
		PARSE_OPERATOR(">", t_to);
		PARSE_OPERATOR("<", t_from);
		PARSE_OPERATOR_PARAMETER(">&", 1, t_to_fd);
		PARSE_OPERATOR_PARAMETER("<&", 1, t_from_fd);
		PARSE_OPERATOR("&&", t_and);
		PARSE_OPERATOR("||", t_or);
		PARSE_OPERATOR("|", t_pipe);
		PARSE_OPERATOR(")", t_unexpected);
		PARSE_OPERATOR("&", t_unknown); // not implemented yet. Will be in 42sh.
		PARSE_CHARACTER("*", t_wildcard);
		PARSE_WORD(WORD_END);
	}

	return head;
}
