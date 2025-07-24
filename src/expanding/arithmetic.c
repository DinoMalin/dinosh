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
		if (str[i] != '+' && str[i] != '-' && str[i] != '/' && str[i] != '=' && str[i] != '%' && str[i] != '*' 
			&& str[i] != '!' && str[i] != '<' && str[i] != '>' && str[i] != '&' && str[i] != '|')
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
		dprintf(2, "dinosh: syntax error: 1operand expected (error token is \"%s\")", str);
		return NULL;
	}
	else if (str[0] == '=' && (str[1] != '\0' && str[1] != '=')) {
		dprintf(2, "dinosh: syntax error: 2operand expected (error token is \"%s\")", str);
		return NULL;
	}
	else if (str[0] == '*' && str[1] != '\0') {
		dprintf(2, "dinosh: syntax error: 3operand expected (error token is \"%s\")", str);
		return NULL;
	} 
	// Comparison operators
	else if (str[0] == '=' && str[1] == '=') 
		return(ft_strdup("=="));
	else if (str[0] == '!' && str[1] == '=')
		return(ft_strdup("!="));
	else if (str[0] == '<' && str[1] == '=')
		return(ft_strdup("<="));
	else if (str[0] == '>' && str[1] == '=')
		return(ft_strdup(">="));
	else if (str[0] == '<' && str[1] == '\0')
		return(ft_strdup("<"));
	else if (str[0] == '>' && str[1] == '\0')
		return(ft_strdup(">"));
	// Logical operators
	else if (str[0] == '&' && str[1] == '&')
		return(ft_strdup("&&"));
	else if (str[0] == '|' && str[1] == '|')
		return(ft_strdup("||"));
	// Arithmetic operators
	else if (str[0] == '=' && str[1] == '\0')
		return(ft_strdup("="));
	else if (str[0] == '*' && str[1] == '\0')
		return(ft_strdup("*"));
	else if (str[0] == '/' && str[1] == '\0')
		return(ft_strdup("/"));
	else if (str[0] == '%' && str[1] == '\0')
		return(ft_strdup("%"));
	
	for (int j = 0; str[j]; j++) {
		if (str[j] != '+' && str[j] != '-') {
			dprintf(2, "dinosh: syntax error: 4operand expected (error token is \"%s\")", str);
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
			str[i] == '%' || str[i] == '=' || str[i] =='*' ||
			str[i] == '!' || str[i] == '<' || str[i] == '>' ||
			str[i] == '&' || str[i] == '|') {
			if (buff_idx > 0) {
				buffer[buff_idx] = '\0';
				tokens[token_count++] = ft_strdup(buffer);
			}
			buff_idx = 0;
			while (str[i] && (str[i] == '+' || str[i] == '-' || str[i] == '/' || 
							str[i] == '%' || str[i] == '=' || str[i] == '*' ||
							str[i] == '!' || str[i] == '<' || str[i] == '>' ||
							str[i] == '&' || str[i] == '|')) {
				buffer[buff_idx++] = str[i++];
			}
			buffer[buff_idx] = '\0';
            tokens[token_count++] = ft_strdup(buffer);
            buff_idx = 0;
		} else {
			while (str[i] && !isspace(str[i]) && str[i] != '+' && str[i] != '-' && str[i] != '/' && 
				str[i] != '%' && str[i] != '=' && str[i] != '*' &&
				str[i] != '!' && str[i] != '<' && str[i] != '>' &&
				str[i] != '&' && str[i] != '|') {
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
	(void)env;
	if (str[0] == '\0') {
		t_arit *result = malloc(sizeof(t_arit));
		result->value = ft_strdup("0");
		result->baseVal = ft_strdup("0");
		result->isEnv = false;
		result->isOp = false;
		result->next = NULL;
		return result;
	}
	char **split = tokenize_arithmetic(str);

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
	bool prev_was_operand = false; 
	for(i = 0; split[i] != NULL ; i++) {
		if (is_only_digit(split[i])) {
			if (prev_was_operand) {
				dprintf(2, "dinosh: %s: syntax error: operator expected (error token is \"%s\")", str, split[i]);
				free_av(split);
				free_arit(head);
				return NULL;
			}
			result->value = ft_strdup(split[i]);
			result->isEnv = false;
			result->isOp = false;
			result = result->next;
			prev_was_operand = true;
			continue;
		} else if (ft_isdigit(split[i][0]) && !is_only_digit(split[i])) {
			dprintf(2, "bash: %s: value too great for base (error token is \"%s\")", split[i], split[i]);
			free_av(split);
			return NULL;
		}
		else if (is_op(split[i])) {
			// Check if this is the last token and it's an operator (syntax error)
			if (split[i + 1] == NULL) {
				dprintf(2, "dinosh: %s: syntax error: operand expected (error token is \"%s\")", str, split[i]);
				free_av(split);
				free_arit(head);
				return NULL;
			}
			result->value = assign_op(split[i]);
			if (!result->value) {
				free_av(split);
				free_arit(head);
				return NULL;
			}
			result->baseVal = ft_strdup(split[i]);
			result->isEnv = false;
			result->isOp = true;
			result = result->next;
			prev_was_operand = false;
			continue;
		}
		else {
			// Check for consecutive operands (missing operator) - variables count as operands too
			if (prev_was_operand) {
				dprintf(2, "dinosh: %s: syntax error: operator expected (error token is \"%s\")", str, split[i]);
				free_av(split);
				free_arit(head);
				return NULL;
			}
			result->value = ft_strdup(split[i]);
			result->isEnv = true;
			result->isOp = false;
			result = result->next;
			prev_was_operand = true;
		}
	}
	free_av(split);
	result = NULL;
	return head;
}

long do_assign(t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	long resVal = 0;
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	if (var1->isEnv) {
		if (var2->isEnv) {
			char *val2 = val_in_env(env, var2->value);
			modify_env(&env, var1->value, val2, 0, -1);
			resVal = ft_atoi_base(val2);
			free(val2);
		} else {
			modify_env(&env, var1->value, var2->value, 0, -1);
			resVal = ft_atoi_base(var2->value);
		}
	} else {
		dprintf(2,  "dinosh: %s: attempted assignment to non-variable (error token is \"%s\")", fullStr, var1->value);
		return ERROR_VALUE;
	}
	return resVal;
}

long do_add(t_arit *var1, t_arit *var2, Env *env)
{
	long resVal = 0;
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
		char *val = val_in_env(env, var2->value);
		resVal += ft_atoi_base(val);
		free(val);
	}
	else
		resVal += ft_atoi_base(var2->value);
	return resVal;
}

long do_minus(t_arit *var1, t_arit *var2, Env *env)
{
	long resVal = 0;
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
		char *val = val_in_env(env, var2->value);
		resVal -= ft_atoi_base(val);
		free(val);
	}
	else
		resVal -= ft_atoi_base(var2->value);
	return resVal;
}

long do_div(t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	long tmp = 0;
	long resVal = 0;
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
		char *val = val_in_env(env, var2->value);
		tmp = ft_atoi_base(val);
		free(val);
	}
	else
		tmp = ft_atoi_base(var2->value);
	if (tmp == 0) {
		dprintf(2, "dinosh: %s: division by 0 (error token is \"0\")", fullStr);
		return ERROR_VALUE;
	}
	resVal /= tmp;
	return resVal;
}

long do_mod(t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	long tmp = 0;
	long resVal = 0;
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
		char *val = val_in_env(env, var2->value);
		tmp = ft_atoi_base(val);
		free(val);
	}
	else
		tmp = ft_atoi_base(var2->value);
	if (tmp == 0) {
		dprintf(2, "dinosh: %s: division by 0 (error token is \"0\")", fullStr);
		return ERROR_VALUE;
	}
	resVal %= tmp;
	return resVal;
}

long do_mul(t_arit *var1, t_arit *var2, Env *env)
{
	long tmp = 0;
	long resVal = 0;
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
		char *val = val_in_env(env, var2->value);
		tmp = ft_atoi_base(val);
		free(val);
	}
	else
		tmp = ft_atoi_base(var2->value);
	resVal *= tmp;
	return resVal;
}

long do_equal(t_arit *var1, t_arit *var2, Env *env)
{
	long val1 = 0, val2 = 0;
	
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
		
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		val1 = ft_atoi_base(val);
		free(val);
	}
	else
		val1 = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var2->value);
		val2 = ft_atoi_base(val);
		free(val);
	}
	else
		val2 = ft_atoi_base(var2->value);
	
	return (val1 == val2) ? 1 : 0;
}

long do_not_equal(t_arit *var1, t_arit *var2, Env *env)
{
	long val1 = 0, val2 = 0;
	
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
		
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		val1 = ft_atoi_base(val);
		free(val);
	}
	else
		val1 = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var2->value);
		val2 = ft_atoi_base(val);
		free(val);
	}
	else
		val2 = ft_atoi_base(var2->value);
	
	return (val1 != val2) ? 1 : 0;
}

long do_less_than(t_arit *var1, t_arit *var2, Env *env)
{
	long val1 = 0, val2 = 0;
	
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
		
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		val1 = ft_atoi_base(val);
		free(val);
	}
	else
		val1 = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var2->value);
		val2 = ft_atoi_base(val);
		free(val);
	}
	else
		val2 = ft_atoi_base(var2->value);
	
	return (val1 < val2) ? 1 : 0;
}

long do_greater_than(t_arit *var1, t_arit *var2, Env *env)
{
	long val1 = 0, val2 = 0;
	
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
		
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		val1 = ft_atoi_base(val);
		free(val);
	}
	else
		val1 = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var2->value);
		val2 = ft_atoi_base(val);
		free(val);
	}
	else
		val2 = ft_atoi_base(var2->value);
	
	return (val1 > val2) ? 1 : 0;
}

long do_less_equal(t_arit *var1, t_arit *var2, Env *env)
{
	long val1 = 0, val2 = 0;
	
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
		
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		val1 = ft_atoi_base(val);
		free(val);
	}
	else
		val1 = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var2->value);
		val2 = ft_atoi_base(val);
		free(val);
	}
	else
		val2 = ft_atoi_base(var2->value);
	
	return (val1 <= val2) ? 1 : 0;
}

long do_greater_equal(t_arit *var1, t_arit *var2, Env *env)
{
	long val1 = 0, val2 = 0;
	
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
		
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		val1 = ft_atoi_base(val);
		free(val);
	}
	else
		val1 = ft_atoi_base(var1->value);

	if (var2->isEnv) {
		char *val = val_in_env(env, var2->value);
		val2 = ft_atoi_base(val);
		free(val);
	}
	else
		val2 = ft_atoi_base(var2->value);
	
	return (val1 >= val2) ? 1 : 0;
}

long do_logical_and(t_arit *var1, t_arit *var2, Env *env)
{
	long val1 = 0, val2 = 0;
	
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
		
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		val1 = ft_atoi_base(val);
		free(val);
	}
	else
		val1 = ft_atoi_base(var1->value);

	// Short-circuit evaluation: if val1 is 0, return 0 without evaluating val2
	if (val1 == 0)
		return 0;

	if (var2->isEnv) {
		char *val = val_in_env(env, var2->value);
		val2 = ft_atoi_base(val);
		free(val);
	}
	else
		val2 = ft_atoi_base(var2->value);
	
	return (val1 && val2) ? 1 : 0;
}

long do_logical_or(t_arit *var1, t_arit *var2, Env *env)
{
	long val1 = 0, val2 = 0;
	
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
		
	if (var1->isEnv) {
		char *val = val_in_env(env, var1->value);
		val1 = ft_atoi_base(val);
		free(val);
	}
	else
		val1 = ft_atoi_base(var1->value);

	// Short-circuit evaluation: if val1 is non-zero, return 1 without evaluating val2
	if (val1 != 0)
		return 1;

	if (var2->isEnv) {
		char *val = val_in_env(env, var2->value);
		val2 = ft_atoi_base(val);
		free(val);
	}
	else
		val2 = ft_atoi_base(var2->value);
	
	return (val1 || val2) ? 1 : 0;
}

int get_precedence(char *op) {
	if (!strcmp(op, "="))
		return 1;
	if (!strcmp(op, "||"))
		return 2;
	if (!strcmp(op, "&&"))
		return 3;
	if (!strcmp(op, "==") || !strcmp(op, "!="))
		return 4;
	if (!strcmp(op, "<") || !strcmp(op, ">") || !strcmp(op, "<=") || !strcmp(op, ">="))
		return 5;
	if (!strcmp(op, "+") || !strcmp(op, "-"))
		return 6;
	if (!strcmp(op, "*") || !strcmp(op, "/") || !strcmp(op, "%"))
		return 7;
	return 0;
}

long evaluate_operation(char *op, t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	if (!strcmp(op, "="))
		return do_assign(var1, var2, env, fullStr);
	else if (!strcmp(op, "+"))
		return do_add(var1, var2, env);
	else if (!strcmp(op, "-"))
		return do_minus(var1, var2, env);
	else if (!strcmp(op, "*"))
		return do_mul(var1, var2, env);
	else if (!strcmp(op, "/"))
		return do_div(var1, var2, env, fullStr);
	else if (!strcmp(op, "%"))
		return do_mod(var1, var2, env, fullStr);
	else if (!strcmp(op, "=="))
		return do_equal(var1, var2, env);
	else if (!strcmp(op, "!="))
		return do_not_equal(var1, var2, env);
	else if (!strcmp(op, "<"))
		return do_less_than(var1, var2, env);
	else if (!strcmp(op, ">"))
		return do_greater_than(var1, var2, env);
	else if (!strcmp(op, "<="))
		return do_less_equal(var1, var2, env);
	else if (!strcmp(op, ">="))
		return do_greater_equal(var1, var2, env);
	else if (!strcmp(op, "&&"))
		return do_logical_and(var1, var2, env);
	else if (!strcmp(op, "||"))
		return do_logical_or(var1, var2, env);
	
	return ERROR_VALUE;
}

long do_op(t_arit *tokens, Env *env, char *fullStr)
{
	if (!tokens)
		return ERROR_VALUE;
	
	// Handle single operand
	if (!tokens->next) {
		if (tokens->isOp)
			return ERROR_VALUE;
		if (tokens->isEnv) {
			char *val = val_in_env(env, tokens->value);
			long result = ft_atoi_base(val);
			free(val);
			return result;
		}
		return ft_atoi_base(tokens->value);
	}
	
	// Multi-pass evaluation by precedence level (7 down to 1 for right associativity)
	for (int precedence = 7; precedence >= 1; precedence--) {
		t_arit *curr = tokens;
		int found_op = 1;
		
		// Keep evaluating operators of this precedence level until none are found
		while (found_op) {
			found_op = 0;
			curr = tokens;
			
			while (curr && curr->next && curr->next->next) {
				if (curr->next->isOp && get_precedence(curr->next->value) == precedence) {
					t_arit *left = curr;
					t_arit *op = curr->next;
					t_arit *right = curr->next->next;
					
					long result = evaluate_operation(op->value, left, right, env, fullStr);
					if (result == ERROR_VALUE)
						return ERROR_VALUE;
					
					// Replace the three nodes with result
					char *result_str = ft_itoa(result);
					free(left->value);
					left->value = result_str;
					left->isEnv = false;
					left->isOp = false;
					
					// Remove operator and right operand
					left->next = right->next;
					free(op->value);
					if (op->baseVal)
						free(op->baseVal);
					free(op);
					free(right->value);
					if (right->baseVal)
						free(right->baseVal);
					free(right);
					
					found_op = 1;
					break; // Start over from the beginning for this precedence level
				}
				curr = curr->next;
			}
		}
	}
	
	// Should have only one token left
	if (tokens && !tokens->next) {
		if (tokens->isEnv) {
			char *val = val_in_env(env, tokens->value);
			long result = ft_atoi_base(val);
			free(val);
			return result;
		}
		return ft_atoi_base(tokens->value);
	}
	
	return ERROR_VALUE;
}

void arithmetic(Env *env, Parser *el)
{
	t_arit *tokens = pre_parse(el->content, env);
	if (!tokens) {
		// Syntax error occurred, set content to empty string to prevent printing original expression
		// The error message was already printed by pre_parse
		free(el->content);
		el->content = ft_strdup("");
		return;
	}
	
	long result = do_op(tokens, env, el->content);
	free_arit(tokens);
	
	if (result == ERROR_VALUE) {
		free(el->content);
		el->content = ft_strdup("0");
		return;
	}
	
	char *result_str = ft_itoa(result);
	free(el->content);
	el->content = result_str;
}