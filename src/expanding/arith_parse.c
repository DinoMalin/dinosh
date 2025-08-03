#include "expand.h"

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
		if (ft_isdigit(str[i])) {
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
	else if (str[0] == '&' && str[1] == '&')
		return(ft_strdup("&&"));
	else if (str[0] == '|' && str[1] == '|')
		return(ft_strdup("||"));
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
			str[i] == '&' || str[i] == '|' || str[i] == '(' || str[i] == ')') {
			if (buff_idx > 0) {
				buffer[buff_idx] = '\0';
				tokens[token_count++] = ft_strdup(buffer);
			}
			buff_idx = 0;
			
			if (str[i] == '(' || str[i] == ')') {
				buffer[0] = str[i];
				buffer[1] = '\0';
				tokens[token_count++] = ft_strdup(buffer);
				i++;
			} else {
				while (str[i] && (str[i] == '+' || str[i] == '-' || str[i] == '/' || 
								str[i] == '%' || str[i] == '=' || str[i] == '*' ||
								str[i] == '!' || str[i] == '<' || str[i] == '>' ||
								str[i] == '&' || str[i] == '|')) {
					buffer[buff_idx++] = str[i++];
				}
				buffer[buff_idx] = '\0';
				tokens[token_count++] = ft_strdup(buffer);
			}
            buff_idx = 0;
		} else {
			while (str[i] && !ft_isspace(str[i]) && str[i] != '+' && str[i] != '-' && str[i] != '/' && 
				str[i] != '%' && str[i] != '=' && str[i] != '*' &&
				str[i] != '!' && str[i] != '<' && str[i] != '>' &&
				str[i] != '&' && str[i] != '|' && str[i] != '(' && str[i] != ')') {
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

int find_matching_paren(char **tokens, int start) {
	int count = 1;
	int i = start + 1;
	
	while (tokens[i] && count > 0) {
		if (!ft_strcmp(tokens[i], "("))
			count++;
		else if (!ft_strcmp(tokens[i], ")"))
			count--;
		i++;
	}
	
	if (count == 0)
		return i - 1; 
	return -1;
}

long evaluate_parentheses(char **tokens, int start, int end, Env *env, char *fullStr) {
	char *sub_expr = malloc(1024);
	sub_expr[0] = '\0';
	
	for (int i = start + 1; i < end; i++) {
		if (i > start + 1)
			ft_strlcat(sub_expr, " ", 1024);
		ft_strlcat(sub_expr, tokens[i], 1024);
	}
	
	t_arit *sub_tokens = pre_parse(sub_expr, env);
	free(sub_expr);
	
	if (!sub_tokens)
		return ERROR_VALUE;
	
	long result = do_op(sub_tokens, env, fullStr);
	free_arit(sub_tokens);
	return result;
}
