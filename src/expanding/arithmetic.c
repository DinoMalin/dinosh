#include "expand.h"

void free_arit(t_arit *content)
{
	while(content) {
		free(content->value);
		if (content->baseVal)
			free(content->baseVal);
		t_arit *tmp = content;
		content = content->next;
		free(tmp);
	}
}

#include <stdbool.h>
#include <ctype.h>

bool is_only_digit(char *str)
{
	int i = 0;
	int base = 10;

	if (str[i] == '0')
	{
		if (str[i + 1] == 'x' || str[i + 1] == 'X')
		{
			base = 16;
			i += 2;
		}
		else if (str[i + 1] == 'b' || str[i + 1] == 'B')
		{
			base = 2;
			i += 2;
		}
		else
		{
			base = 8;
			i++;
		}
	}

	for (; str[i] != '\0'; i++)	{
		if (isdigit(str[i])) {
			if (str[i] - '0' >= base)
				return false;
		} else if (base == 16 && ((str[i] >= 'a' && str[i] <= 'f') || (str[i] >= 'A' && str[i] <= 'F')))
			continue;
		else if (base == 2 && (str[i] == '0' || str[i] == '1'))
			continue;
		else if (base == 8 && (str[i] >= '0' && str[i] <= '7'))
			continue;
		else
			return false;
	}
	return true;
}


bool is_op(char *str)
{
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] != '+' && str[i] != '-' && str[i] != '/' && str[i] != '=' && str[i] != '%')
			return(false);
	}
	return(true);
}

char *var_in_env(Env *env, char *str)
{
	Env *tmp = env;
	while (tmp)	{
		if (!ft_strcmp(tmp->var, str)) {
			return (ft_strdup(tmp->var));
		}
		tmp = tmp->next;
	}
	return (NULL);
}

char *val_in_env(Env *env, char *str)
{
	Env *tmp = env;
	while (tmp)	{
		if (!ft_strcmp(tmp->var, str)) {
			return (ft_strdup(tmp->value));
		}
		tmp = tmp->next;
	}
	return (ft_strdup("0"));
}

char *assign_op(char *str)
{
	int sign = 1;

	if ((str[0] == '/' || str[0] == '%') && str[1] != '\0') {
		dprintf(2, " syntax error: operand expected (error token is \"%s\")", str);
		return NULL;
	}
		else if (str[0] == '=' && (str[1] != '\0' && str[1] != '=')) {
		dprintf(2, " syntax error: operand expected (error token is \"%s\")", str);
		return NULL;
	}
	else if (str[0] == '=' && str[1] == '=') 
		return(ft_strdup("=="));
	else if  (str[0] == '=' && str[1] == '\0')
		return(ft_strdup("="));
	for (int j = 0; str[j]; j++) {
		if (str[j] != '+' && str[j] != '-') {
			dprintf(2, " syntax error: operand expected (error token is \"%s\")", str);
			return NULL;
		}
		if (str[j] == '-')
			sign *= -1;
	}
	if (sign == -1)
		return (ft_strdup("-"));
	else
	return (ft_strdup("+"));
}

char **tokenize_arithmetic(char *str)
{
	if (!str)
		return NULL;
	char **tokens = malloc((ft_strlen(str) + 1) * sizeof(char *));
	char buffer[1024];
	int i = 0;
	int buff_idx = 0;
	int token_count = 0;

	while (str[i]) {
		if (ft_isspace(str[i])){
			i++;
			continue;
		}

		if (str[i] == '+' || str[i] == '-' || str[i] == '/' || 
			str[i] == '%' || str[i] == '=') {
			if (buff_idx > 0) {
				buffer[buff_idx] = '\0';
				tokens[token_count++] = ft_strdup(buffer);
			}
			buff_idx = 0;
			while (str[i] && (str[i] == '+' || str[i] == '-' || str[i] == '/' || 
							str[i] == '%' || str[i] == '=')) {
				buffer[buff_idx++] = str[i++];
			}
			buffer[buff_idx] = '\0';
            tokens[token_count++] = ft_strdup(buffer);
            buff_idx = 0;
		} else {
			while (str[i] && !isspace(str[i]) && str[i] != '+' && str[i] != '-' && str[i] != '/' && 
				str[i] != '%' && str[i] != '=') {
				buffer[buff_idx++] = str[i++];
			}
            if (buff_idx > 0) {
                buffer[buff_idx] = '\0';
                tokens[token_count++] = ft_strdup(buffer);
                buff_idx = 0;
            }
		}
	}

	tokens[token_count] = NULL;
	return tokens;
}
t_arit *pre_parse(char *str, Env *env)
{
	int len = 0;
	int i = 0;
	char **split = tokenize_arithmetic(str);
	if (!split) {
		t_arit *result = malloc(sizeof(t_arit *));
		result->value = ft_strdup("0");
		result->baseVal = ft_strdup("0");
		result->isEnv = false;
		result->isOp = false;
		result->next = NULL;
		return result;
	}

	for(int i = 0; split[i]; i++)
	len++;
	t_arit *head = NULL;
	t_arit *prev = NULL;
	for (int i = 0; split[i]; i++) {
		t_arit *new_node = ft_calloc(1, sizeof(t_arit));
		new_node->next = NULL;
		if (!head)
			head = new_node;
		if (prev)
			prev->next = new_node;
		prev = new_node;
	}
	t_arit *result = head;

	for(i = 0; split[i] != NULL ; i++) {
		if (is_only_digit(split[i])) {
			result->value = ft_strdup(split[i]);
			result->isEnv = false;
			result->isOp = false;
			result = result->next;
			continue;
		} else if (ft_isdigit(split[i][0]) && !is_only_digit(split[i])) {
			dprintf(2, "bash: %s: value too great for base (error token is \"%s\")", split[i], split[i]);
			free_av(split);
			return NULL;
		}
		else if (is_op(split[i])) {
			result->value = assign_op(split[i]);
			if (!result->value) {
				free_av(split);
				return NULL;
			}
			result->baseVal = ft_strdup(split[i]);
			result->isEnv = false;
			result->isOp = true;
			result = result->next;
			continue;
		}
		else {
			result->value = var_in_env(env, split[i]);
			result->value = ft_strdup(split[i]);
			result->isEnv = true;
			result->isOp = false;
			result = result->next;
		}
	}
	free_av(split);
	result = NULL;
	return head;
}

long do_assign(t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	int resVal = 0;
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	if (var1->isEnv) {
		if (var2->isEnv) {
			modify_env(&env, var1->value, val_in_env(env, var2->value), 0, -1);
			resVal = ft_atoi_base(val_in_env(env, var2->value));
		} else {
			modify_env(&env, var1->value, var2->value, 0, -1);
			resVal = ft_atoi_base(var2->value);
		}
	} else {
		dprintf(2,  "dinosh: %s: attempted assignment to non-variable (error token is \"%s\")", fullStr, var1->value);
		return -1;
	}
	return resVal;
}

long do_add(t_arit *var1, t_arit *var2, Env *env)
{
	int resVal = 0;
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		resVal = ft_atoi_base(val);
		free(val);
	}
	else
		resVal = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var1->value);
		resVal += ft_atoi_base(val);
		free(val);
	}
	else
		resVal += ft_atoi_base(var2->value);
	return resVal;
}

long do_minus(t_arit *var1, t_arit *var2, Env *env)
{
	int resVal = 0;
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		resVal = ft_atoi_base(val);
		free(val);
	}
	else
		resVal = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var1->value);
		resVal -= ft_atoi_base(val);
		free(val);
	}
	else
		resVal -= ft_atoi_base(var2->value);
	return resVal;
}

long do_div(t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	int tmp = 0;
	int resVal = 0;
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		resVal = ft_atoi_base(val);
		free(val);
	}
	else
		resVal = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var1->value);
		tmp = ft_atoi_base(val);
		free(val);
	}
	else
		tmp = ft_atoi_base(var2->value);
	if (resVal == 0) {
		dprintf(2, "dinosh: %s: division by 0 (error token is \"0\")", fullStr);
		return ERROR_VALUE;
	}
	resVal /= tmp;
	return resVal;
}

long do_mod(t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	int tmp = 0;
	int resVal = 0;
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		resVal = ft_atoi_base(val);
		free(val);
	}
	else
		resVal = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var1->value);
		tmp = ft_atoi_base(val);
		free(val);
	}
	else
		tmp = ft_atoi_base(var2->value);
	if (resVal == 0) {
		dprintf(2, "dinosh: %s: division by 0 (error token is \"0\")", fullStr);
		return ERROR_VALUE;
	}
	resVal %= tmp;
	return resVal;
}

char *do_op(t_arit *content, Env *env, char *fullStr) {
	long tmp = 0;
	t_arit *head = content;

    if (!content->next) {
        if (content->isOp) {
            dprintf(2, "dinosh: %s: syntax error: operand expected (error token is \"%s\")", 
                    fullStr, content->value);
            return NULL;
        } else if (content->isEnv) {
            return val_in_env(env, content->value);
        } else {
            return ft_itoa(ft_atoi_base(content->value));
        }
    }
	
	content->prev = content;
	while (content && content->next)
	{
		if (!content->isOp && content->next && content->next->isOp && content->next->next) {
			t_arit *op = content->next;
			t_arit *operand2 = content->next->next;

			if (operand2->isOp) {
				dprintf(2,  "dinosh: %s: syntax error in expression (error token is \"%s\")", fullStr, content->next->value);
				return NULL;
			}
			if (!strcmp(op->value, "=")) {
				tmp = do_assign(content, operand2, env, fullStr);
			} else if (!strcmp(op->value, "+")) {
				tmp = do_add(content, operand2, env);
			} else if (!strcmp(op->value, "-")) {
				tmp = do_minus(content, operand2, env);
			} else if (!strcmp(op->value, "/")) {
				tmp = do_div(content, operand2, env, fullStr);
			} else if (!strcmp(op->value, "%")) {
				tmp = do_mod(content, operand2, env, fullStr);
			}
			
			if (tmp == ERROR_VALUE)
				return NULL;
			free(content->value);
			content->value = ft_itoa(tmp);
			content->isEnv = false;
			content->isOp = false;
			if (operand2->next) {
                content->next = operand2->next;
                operand2->next = NULL;
                free_arit(op);
            } else {
                content->next = NULL;
                free_arit(op);
            }
			} else {
				content = content->next;
			}
		}
	return ft_strdup(head->value);
}

void arithmetic(Env *env, Parser *el) {
	char *s = el->content;
	t_arit *content = pre_parse(el->content, env);
	if (!content) {
		el->content = ft_strdup("");
		return ;
	}
	t_arit *tmp = content;
	el->content = do_op(tmp, env, s);
	free_arit(content);
	free(s);
}
