#include "minishell.h"

Env *create_env(char **env) {
	Env *head = NULL;
	Env *last = NULL;

	for (int i = 0; env[i]; i++) {
		Env *new = ft_calloc(1, sizeof(Env));

		int equal = ft_strchr(env[i], '=') - env[i];
		int len_value = ft_strlen(env[i]) - equal;
		new->var = ft_substr(env[i], 0, equal);
		new->value = ft_substr(env[i], equal + 1, len_value);
		new->durability = -1;

		if (!i) {
			head = new;
			last = new;
		} else {
			last->next = new;
			last = new;
		}
	}

	return head;
}

void modify_env(Env **env, char *target, char *new_value, Special special, int dur) {
	Env *curr = *env;
	Env *last = curr;

	while (curr) {
		if (!ft_strcmp(target, curr->var)) {
			if (curr->durability == -1 && dur != -1)
				curr->old_value = curr->value;
			else
				free(curr->value);
			curr->value = ft_strdup(new_value);
			curr->type = special;
			curr->durability = dur;
			return;
		}
		last = curr;
		curr = curr->next;
	}

	Env *new = ft_calloc(1, sizeof(Env));
	new->var = ft_strdup(target);
	new->value = ft_strdup(new_value);
	new->type = special;
	new->durability = dur;

	if (!*env) {
		*env = new;
	} else {
		last->next = new;
	}
}

void delete_var(Env **env, char *target) {
	Env *curr = *env;
	Env *last = *env;

	while (curr) {
		if (!ft_strcmp(target, curr->var)) {
			if (curr == *env)
				*env = curr->next;
			else
				last->next = curr->next;

			free(curr->var);
			free(curr->old_value);
			free(curr->value);
			free(curr);

			return;
		}
		last = curr;
		curr = curr->next;
	}
}
