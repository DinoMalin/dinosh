#include "minishell.h"

#define DEFAULT_CHECK(s, g)	\
	{						\
		if (*s != g)		\
			return false;	\
		s++;				\
	}

static char *bracket_end(char *bracket) {
	bool escape = false;
	while (*bracket) {
		if (!escape && *bracket == ']')
			break;
		if (!escape && *bracket == '\\')
			escape = true;
		else
			escape = false;
		bracket++;
	}
	return bracket;
}

static bool is_in_bracket(char c, char *bracket) {
	bool res = true;

	if (*bracket == '!')
		res = false;
	while (*bracket) {
		if (*bracket == '\\') {
			if (*bracket == c)
				return res;
			bracket++;
		} else if (*(bracket+1) == '-' && *(bracket+2)) {
			if (c >= *bracket && c <= *(bracket+2))
				return res;
			bracket = bracket+2;
		} else if (*bracket == c)
			return res;
		bracket++;
	}
	return !res;
}

static bool its_a_match(char *str, char *pattern) {
	if (!*str) {
		while (*pattern == '*')
			pattern++;
	}

	while (*str && *pattern) {
		char glob = *pattern;
		char *end = NULL;
		pattern++;

		switch (glob) {
			case '\\':
			DEFAULT_CHECK(str, *pattern);
			pattern++;
			break;

			case '*':
			while (*pattern == '*')
				pattern++;
			while (*str && *str != *pattern)
				str++;
			while (*str && *(str+1) == *str)
				str++;
			break;

			case '?':
			str++;
			break;

			case '[':
			end = bracket_end(pattern);
			if (*end) {
				if (!is_in_bracket(*str, pattern))
					return false;
				else {
					str++;
					pattern = end+1;
				}
				break;
			} __attribute__ ((fallthrough));

			default:
			DEFAULT_CHECK(str, glob);

		}
	}
	return (!*str && !*pattern);
}

bool test_prefix(char *str, char *pattern, int index) {
	char c = str[index+1];
	str[index+1] = '\0';
	bool match = its_a_match(str, pattern);
	str[index+1] = c;
	return match;
}

#define LARGEST_SUFFIX()								\
	{													\
		int i = 0;										\
		for (; str[i]; i++) {							\
			if (its_a_match(str+i, pattern+1)) {		\
				break;									\
			}											\
		}												\
		if (its_a_match("", pattern+1))					\
			res = ft_substr(str, 0, ft_strlen(str));	\
		else											\
			res = ft_substr(str, 0, i);					\
	}

#define SMALLEST_SUFFIX()								\
	{													\
		int i = 0;										\
		for (; str[i]; i++) {							\
			if (its_a_match(str+i, pattern)) {			\
				break;									\
			}											\
		}												\
		for (; str[i]; i++) {							\
			if (!its_a_match(str+i, pattern)) {			\
				break;									\
			}											\
		}												\
		if (i != 0)										\
			i--;										\
		if (its_a_match("", pattern))					\
			res = ft_substr(str, 0, ft_strlen(str));	\
		else											\
			res = ft_substr(str, 0, i);					\
	}

#define LARGEST_PREFIX()									\
	{														\
		int i = 0;											\
		for (; str[i]; i++) {								\
			if (test_prefix(str, pattern+1, i)) {			\
				i++;										\
				break;										\
			}												\
		}													\
		for (; str[i]; i++) {								\
			if (!test_prefix(str, pattern+1, i)) {			\
				break;										\
			}												\
		}													\
		if (its_a_match("", pattern+1))						\
			res = ft_substr(str, 0, ft_strlen(str));		\
		else												\
			res = ft_substr(str+i, 0, ft_strlen(str+i));	\
	}

#define SMALLEST_PREFIX()									\
	{														\
		int i = 0;											\
		bool found = false;									\
		for (; str[i]; i++) {								\
			if (test_prefix(str, pattern, i)) {				\
				i++;										\
				found = true;								\
				break;										\
			}												\
		}													\
		if (its_a_match("", pattern))						\
			res = ft_substr(str, 0, ft_strlen(str));		\
		else if (found)										\
			res = ft_substr(str+i, 0, ft_strlen(str+i));	\
		else												\
			res = ft_strdup(str);							\
	}

char *resolve_globing(char *str, char *pattern, bool suffix) {
	char *res = NULL;
	if (suffix) {
		if (*pattern == '%') {
			LARGEST_SUFFIX();
		} else {
			SMALLEST_SUFFIX();
		}
	} else {
		if (*pattern == '#') {
			LARGEST_PREFIX();
		} else {
			SMALLEST_PREFIX();
		}
	}

	free(str);
	free(pattern);
	return res;
}
