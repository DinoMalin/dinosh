#include "expand.h"

#define HANDLE_SUBSTITUTION(c, fun)						\
	{													\
		if (*word == c) {								\
			char *w = ft_strdup(word+1);				\
			free(word);									\
			return fun(env, cmd, parameter, w);			\
		}												\
	}

char *replace(Env *env, Command *cmd, char *parameter, char *word) {
	(void)cmd;
	char *var = ft_getenv(env, parameter);
	free(parameter);

	if (!var || !ft_strlen(var))
		return word;

	free(word);
	return ft_strdup(var);
}

char *assign(Env *env, Command *cmd, char *parameter, char *word) {
	(void)cmd;
	char *var = ft_getenv(env, parameter);

	if (!var || !ft_strlen(var)) {
		Env *varenv = getvar(env, parameter);
		Special type = varenv ? varenv->type : INTERN;
		modify_env(&env, parameter, word, type, -1);
		free(parameter);
		return word;
	}

	free(parameter);
	free(word);
	return ft_strdup(var);
}

char *check(Env *env, Command *cmd, char *parameter, char *word) {
	char *var = ft_getenv(env, parameter);

	if (!var || !ft_strlen(var)) {
		cmd->error_message = ft_strdup(parameter);
		cmd->error_message = clean_join(cmd->error_message, ": ");
		cmd->error_message = clean_join(cmd->error_message, word);
		cmd->error = special;
		free(parameter);
		free(word);
		return NULL;
	}

	return ft_strdup(var);
}

char *add(Env *env, Command *cmd, char *parameter, char *word) {
	(void)cmd;
	char *var = ft_getenv(env, parameter);
	free(parameter);

	if (var)
		return word;

	free(word);
	return ft_strdup("");
}

char *substitution(Env *env, Command *cmd, char *parameter, char *word) {
	HANDLE_SUBSTITUTION('-', replace);
	HANDLE_SUBSTITUTION('=', assign);
	HANDLE_SUBSTITUTION('?', check);
	HANDLE_SUBSTITUTION('+', add);

	free(parameter);
	free(word);
	return NULL;
}

bool check_var(char *var) {
	return (int)ft_strlen(var) == get_variable_size(var);
}

char *get_value(Env *env, Command *cmd, char *str) {
	char *colon = ft_strchr(str, ':');
	char *hashtag = ft_strchr(str, '#');
	char *percent = ft_strchr(str, '%');

	if ((colon && hashtag) || (colon && percent) || (hashtag && percent))
		return NULL;

	if (hashtag == str) {
		if (!check_var(str+1))
			return NULL;
		char *var = ft_getenv(env, str+1);
		int len = var ? ft_strlen(var) : 0;
		return ft_itoa(len);
	}
	if (colon) {
		char *parameter = ft_substr(str, 0, colon-str);
		if (!check_var(parameter)) {
			free(parameter);
			return NULL;
		}
		char *word = ft_substr(colon+1, 0, ft_strlen(colon+1));
		return substitution(env, cmd, parameter, word);
	}
	if (percent) {
		char *parameter = ft_substr(str, 0, percent-str);
		if (!check_var(parameter)) {
			free(parameter);
			return NULL;
		}
		char *var = ft_getenv_alloc(env, parameter);
		char *word = ft_substr(percent+1, 0, ft_strlen(percent+1));
		free(parameter);
		return resolve_globing(var, word, true);
	}
	if (hashtag) {
		char *parameter = ft_substr(str, 0, hashtag-str);
		if (!check_var(parameter)) {
			free(parameter);
			return NULL;
		}
		char *var = ft_getenv_alloc(env, parameter);
		char *word = ft_substr(hashtag+1, 0, ft_strlen(hashtag+1));
		free(parameter);
		return resolve_globing(var, word, false);
	}
	return ft_getenv_alloc(env, str);
}

void parameter(Env *env, Command *cmd, Parser *el, int max) {
	char *value = get_value(env, cmd, el->content);
	if (!value) {
		if (!cmd->error)
			cmd->error = bad_substitution;
		return ;
	}

	if (el->quoting == doubles) {
		free(el->content);
		el->content = value;
	} else {
		add_tokenized_args(el, value, max);
		free(value);
	}
}
