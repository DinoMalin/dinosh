#include "expand.h"

long get_val(t_arit *var, Env *env)
{
	if (var->isEnv) {
		char *val = val_in_env(env, var->value);
		long result = ft_atoi_base(val);
		free(val);
		return result;
	}
	return ft_atoi_base(var->value);
}

long cmp_val(t_arit *var1, t_arit *var2, Env *env, int (*cmp)(long, long))
{
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	
	long val1 = get_val(var1, env);
	long val2 = get_val(var2, env);
	
	return cmp(val1, val2) ? 1 : 0;
}

int equal_cmp(long a, long b) { return a == b; }
int not_equal_cmp(long a, long b) { return a != b; }
int less_than_cmp(long a, long b) { return a < b; }
int greater_than_cmp(long a, long b) { return a > b; }
int less_equal_cmp(long a, long b) { return a <= b; }
int greater_equal_cmp(long a, long b) { return a >= b; }

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
