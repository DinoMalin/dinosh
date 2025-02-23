#include "expand.h"

char *replace(Env *env, char *parameter, char *word) {
	char *var = ft_getenv(env, parameter);
	free(parameter);

	if (!var || !ft_strlen(var))
		return word;

	free(word);
	return ft_strdup(var);
}

char *assign(Env *env, char *parameter, char *word) {
	char *var = ft_getenv(env, parameter);

	if (!var || !ft_strlen(var)) {
		Env *varenv = getvar(env, parameter);
		Special type = varenv ? varenv->type : INTERN;
		modify_env(&env, parameter, word, type, -1);
		free(parameter);
		return word;
	}

	free(word);
	return ft_strdup(var);
}

char *substitution(Env *env, char *parameter, char *word) {
	if (*word == '-') {
		char *res = ft_strdup(word+1);
		free(word);
		return replace(env, parameter, res);
	}
	if (*word == '=') {
		char *res = ft_strdup(word+1);
		free(word);
		return assign(env, parameter, res);
	}

	free(parameter);
	free(word);
	return NULL;
}

char *get_value(Env *env, char *str) {
	char *colon = ft_strchr(str, ':');
	if (colon) {
		return substitution(env,
					ft_substr(str, 0, colon-str),
					ft_substr(colon+1, 0, ft_strlen(colon+1))
				);
	} else
		return ft_getenv_alloc(env, str);
	return ft_strdup("");
}

Error expand_parameter(Env *env, Parser *el, int max) {
	char *value = get_value(env, el->content);
	if (!value)
		return bad_substitution;
	free(el->content);
	el->content = ft_strdup("");
	TOKENIZE_VAR();
	free(value);
	return no_error;
}
