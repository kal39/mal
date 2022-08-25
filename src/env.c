#include "env.h"
#include "common.h"

Env *env_create(Env *outer) {
	Env *env = malloc(sizeof(Env));
	env->outer = outer;
	env->table = table_create();
	return env;
}

void env_destroy(Env *env) {
	table_destroy(env->table);
	free(env);
}

void env_set(Env *env, Value *key, Value *value) {
	if (!IS_SYMBOL(key)) return;
	table_set(env->table, AS_STRING(key), value);
}

Value *env_get(Env *env, Value *key) {
	if (!IS_SYMBOL(key)) return NULL;
	Value *value = table_get(env->table, AS_STRING(key));
	return IS_NIL(value) ? env->outer == NULL ? MAKE_ERROR("symbol not found", NULL, key) : env_get(env->outer, key) : value;
}

void env_print(Env *env) {
	table_print(env->table);
	printf("\n");
	if (env->outer != NULL) env_print(env->outer);
}