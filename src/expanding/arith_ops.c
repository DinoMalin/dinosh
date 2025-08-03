#include "expand.h"

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
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	return get_val(var1, env) + get_val(var2, env);
}

long do_minus(t_arit *var1, t_arit *var2, Env *env)
{
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	return get_val(var1, env) - get_val(var2, env);
}

long do_div(t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	
	long div = get_val(var2, env);
	if (div == 0) {
		dprintf(2, "dinosh: %s: division by 0 (error token is \"0\")", fullStr);
		return ERROR_VALUE;
	}
	return get_val(var1, env) / div;
}

long do_mod(t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	
	long div = get_val(var2, env);
	if (div == 0) {
		dprintf(2, "dinosh: %s: division by 0 (error token is \"0\")", fullStr);
		return ERROR_VALUE;
	}
	return get_val(var1, env) % div;
}

long do_mul(t_arit *var1, t_arit *var2, Env *env)
{
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	return get_val(var1, env) * get_val(var2, env);
}

long do_equal(t_arit *var1, t_arit *var2, Env *env)
{
	return cmp_val(var1, var2, env, equal_cmp);
}

long do_not_equal(t_arit *var1, t_arit *var2, Env *env)
{
	return cmp_val(var1, var2, env, not_equal_cmp);
}

long do_less_than(t_arit *var1, t_arit *var2, Env *env)
{
	return cmp_val(var1, var2, env, less_than_cmp);
}

long do_greater_than(t_arit *var1, t_arit *var2, Env *env)
{
	return cmp_val(var1, var2, env, greater_than_cmp);
}

long do_less_equal(t_arit *var1, t_arit *var2, Env *env)
{
	return cmp_val(var1, var2, env, less_equal_cmp);
}

long do_greater_equal(t_arit *var1, t_arit *var2, Env *env)
{
	return cmp_val(var1, var2, env, greater_equal_cmp);
}

long do_logical_and(t_arit *var1, t_arit *var2, Env *env)
{
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	
	long val1 = get_val(var1, env);
	if (val1 == 0)
		return 0;
	
	long val2 = get_val(var2, env);
	return (val1 && val2) ? 1 : 0;
}

long do_logical_or(t_arit *var1, t_arit *var2, Env *env)
{
	if (var1->isOp || var2->isOp)
		return ERROR_VALUE;
	
	long val1 = get_val(var1, env);
	if (val1 != 0)
		return 1;
	
	long val2 = get_val(var2, env);
	return (val1 || val2) ? 1 : 0;
}

int get_precedence(char *op) {
	if (!ft_strcmp(op, "="))
		return 1;
	if (!ft_strcmp(op, "||"))
		return 2;
	if (!ft_strcmp(op, "&&"))
		return 3;
	if (!ft_strcmp(op, "==") || !ft_strcmp(op, "!="))
		return 4;
	if (!ft_strcmp(op, "<") || !ft_strcmp(op, ">") || !ft_strcmp(op, "<=") || !ft_strcmp(op, ">="))
		return 5;
	if (!ft_strcmp(op, "+") || !ft_strcmp(op, "-"))
		return 6;
	if (!ft_strcmp(op, "*") || !ft_strcmp(op, "/") || !ft_strcmp(op, "%"))
		return 7;
	return 0;
}

long evaluate_operation(char *op, t_arit *var1, t_arit *var2, Env *env, char *fullStr)
{
	if (!ft_strcmp(op, "="))
		return do_assign(var1, var2, env, fullStr);
	else if (!ft_strcmp(op, "+"))
		return do_add(var1, var2, env);
	else if (!ft_strcmp(op, "-"))
		return do_minus(var1, var2, env);
	else if (!ft_strcmp(op, "*"))
		return do_mul(var1, var2, env);
	else if (!ft_strcmp(op, "/"))
		return do_div(var1, var2, env, fullStr);
	else if (!ft_strcmp(op, "%"))
		return do_mod(var1, var2, env, fullStr);
	else if (!ft_strcmp(op, "=="))
		return do_equal(var1, var2, env);
	else if (!ft_strcmp(op, "!="))
		return do_not_equal(var1, var2, env);
	else if (!ft_strcmp(op, "<"))
		return do_less_than(var1, var2, env);
	else if (!ft_strcmp(op, ">"))
		return do_greater_than(var1, var2, env);
	else if (!ft_strcmp(op, "<="))
		return do_less_equal(var1, var2, env);
	else if (!ft_strcmp(op, ">="))
		return do_greater_equal(var1, var2, env);
	else if (!ft_strcmp(op, "&&"))
		return do_logical_and(var1, var2, env);
	else if (!ft_strcmp(op, "||"))
		return do_logical_or(var1, var2, env);
	
	return ERROR_VALUE;
}
