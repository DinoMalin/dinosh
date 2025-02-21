#include "parsing.h"

bool skip_whitespace(char **str) {
	bool res = false;
	while (ft_isspace(**str)) {
		(*str)++;
		res = true;
	}
	return res;
}

char *until_escaped(char **str, char *sep) {
	int i = 0;
	bool escape = false;
	int seplen = ft_strlen(sep);
	char *res = ft_strdup("");
	char buff[B_SIZE] = {};
	char *s = *str;

	while (*s) {
		if (!escape && !ft_strncmp(s, sep, seplen))
			break;
		if (*s == '\\' && !ft_strncmp(s+1, sep, seplen))
			escape = true;
		else {
			escape = false;
			buff[i] = *s;
			i++;
		}

		if (i+1 == B_SIZE) {
			res = clean_join(res, buff);
			bzero(buff, B_SIZE);
			i = 0;
		}
		s++;
	}
	res = clean_join(res, buff);
	if (!*s) {
		free(res);
		return NULL;
	}

	*str = s + ft_strlen(sep);

	return res;
}

char *until(char **str, char *sep) {
	char *end = ft_strnstr(*str, sep, ft_strlen(*str));
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
		PARSE_ESCAPED_TOKEN("\"", "\"", t_double_quotes);
		PARSE_TOKEN("'", "'", t_single_quotes);
		PARSE_TOKEN("(", ")", t_subshell);
		PARSE_TOKEN("${", "}", t_var);
		PARSE_OPERATOR_APARAMETER("\\", 1, t_backslash, false);
		PARSE_OPERATOR(";", t_semicolon);
		PARSE_OPERATOR(">&", t_missing_parameter);
		PARSE_OPERATOR("<&", t_missing_parameter);
		PARSE_OPERATOR(">>", t_append);
		PARSE_OPERATOR("<<", t_heredoc);
		PARSE_OPERATOR(">", t_to);
		PARSE_OPERATOR("<", t_from);
		PARSE_OPERATOR_BPARAMETER(">&", 1, t_to_fd, true);
		PARSE_OPERATOR_BPARAMETER("<&", 1, t_from_fd, true);
		PARSE_OPERATOR("&&", t_and);
		PARSE_OPERATOR("||", t_or);
		PARSE_OPERATOR("|", t_pipe);
		PARSE_OPERATOR(")", t_unexpected);
		PARSE_OPERATOR("&", t_bg);
		PARSE_CHARACTER("*", t_wildcard);
		PARSE_WORD(WORD_END);
	}

	return head;
}
