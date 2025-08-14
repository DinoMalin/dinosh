#include "expand.h"

t_arit *pre_parse(char *str, Env *env)
{
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

	bool found_parens = true;
	while (found_parens) {
		found_parens = false;
		for (int i = 0; split[i]; i++) {
			if (!ft_strcmp(split[i], "(")) {
				int close_paren = find_matching_paren(split, i);
				if (close_paren == -1) {
					dprintf(2, "dinosh: %s: syntax error: unmatched '('", str);
					free_av(split);
					return NULL;
				}
				long long paren_result = evaluate_parentheses(split, i, close_paren, env, str);
				if (paren_result == (long long)ERROR_VALUE) {
					free_av(split);
					return NULL;
				}
				char *result_str = ft_ltoa(paren_result);
				free(split[i]);
				split[i] = result_str;
				
				for (int j = i + 1; j <= close_paren; j++) {
					free(split[j]);
					split[j] = NULL;
				}
				
				int write_pos = i + 1;
				int read_pos = close_paren + 1;
				while (split[read_pos]) {
					split[write_pos] = split[read_pos];
					split[read_pos] = NULL;
					write_pos++;
					read_pos++;
				}
				split[write_pos] = NULL;
				
				found_parens = true;
				break;
			}
		}
	}

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
		else if (!ft_strcmp(split[i], ")")) {
			dprintf(2, "dinosh: %s: syntax error: unmatched ')'", str);
			free_av(split);
			free_arit(head);
			return NULL;
		}
		else {
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

long long do_op(t_arit *tokens, Env *env, char *fullStr)
{
	if (!tokens)
		return ERROR_VALUE;
	
	if (!tokens->next) {
		if (tokens->isOp)
			return ERROR_VALUE;
		return get_val(tokens, env);
	}
	
	for (int precedence = 7; precedence >= 1; precedence--) {
		t_arit *curr = tokens;
		int found_op = 1;
		
		while (found_op) {
			found_op = 0;
			curr = tokens;
			
			while (curr && curr->next && curr->next->next) {
				if (curr->next->isOp && get_precedence(curr->next->value) == precedence) {
					t_arit *left = curr;
					t_arit *op = curr->next;
					t_arit *right = curr->next->next;
					
					long long result = evaluate_operation(op->value, left, right, env, fullStr);
					if (result == (long long)ERROR_VALUE)
						return ERROR_VALUE;
					
					char *result_str = ft_ltoa(result);
					free(left->value);
					left->value = result_str;
					left->isEnv = false;
					left->isOp = false;
					
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
					break;
				}
				curr = curr->next;
			}
		}
	}
	
	if (tokens && !tokens->next) {
		return get_val(tokens, env);
	}
	
	return ERROR_VALUE;
}

void arithmetic(Env *env, Parser *el)
{
	t_arit *tokens = pre_parse(el->content, env);
	if (!tokens) {
		free(el->content);
		el->content = ft_strdup("");
		return;
	}
	
	long long result = do_op(tokens, env, el->content);
	free_arit(tokens);
	
	if (result == (long long)ERROR_VALUE) {
		free(el->content);
		el->content = ft_strdup("0");
		return;
	}
	
	char *result_str = ft_ltoa(result);
	free(el->content);
	el->content = result_str;
}
