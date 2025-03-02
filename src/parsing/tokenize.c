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

char *until_nested(char **str, char *start, char *sep) {
	char *end = *str;
	int nested = 1;

	int start_len = ft_strlen(start);
	int end_len = ft_strlen(sep);
	int len = ft_strlen(*str);

	while (*end) {
		if (len >= start_len && !ft_strncmp(end, start, start_len)) {
				nested++;
				end += start_len;
		} else if (len >= end_len && !ft_strncmp(end, sep, end_len)) {
			nested--;
			if (nested == 0) {
				break;
			}
			end += end_len;
		} else {
			end++;
			len = ft_strlen(end);
		}
	}
	if (nested != 0)
		return NULL;

	char *res = ft_substr(*str, 0, end - *str);
	*str = end + ft_strlen(sep);
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

char *parse_word(char **str, char *word_end, int quoting) {
	char *end = *str;
	while (*end && !ft_strchr(word_end, *end)) {
		if (!quoting && ft_isspace(*end))
			break;
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
		PARSE_MINI();
	}

	return head;
}

Parser *tokenize(char *str) {
	int id = 0;
	Parser *curr = NULL;
	Parser *head = NULL;

	bool double_quotes = false;
	bool single_quotes = false;
	bool escape = false;

	while (*str) {
		if (!double_quotes && !single_quotes && !escape && skip_whitespace(&str))
			id++;

		CHECK_QUOTING();
		if (escape) {
			escape = false;
			PARSE_LEN(1, t_word, true);
		}

		if (!single_quotes) {
			PARSE_TOKEN("${", "}", t_var);
			PARSE_TOKEN("$((", "))", t_arithmetic);
			PARSE_TOKEN_NESTED("$(", ")", t_control_substitution);
		}
		if (!single_quotes && !double_quotes) {
			PARSE_TOKEN("(", ")", t_subshell);
			PARSE_TOKEN("{", "}", t_control_group);
			PARSE_OPERATOR("\\\0", t_unexpected);
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
			PARSE_CHARACTER("~", t_tilde);
		}
		PARSE_WORD();
	}

	if (double_quotes || single_quotes || escape == 1) {
		head->error = unclosed_token;
		head->quoting = ADAPTED_QUOTING();
		head->escaped = escape;
	}

	return head;
}
