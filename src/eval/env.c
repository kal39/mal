#include "env.h"

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

void env_add(Env *env, Value *key, Value *value) {
	if (!IS_SYMBOL(key)) return;
	table_add(env->table, AS_STRING(key), value);
}

Value *env_get(Env *env, Value *key) {
	if (!IS_SYMBOL(key)) return NULL;
	return table_get(env->table, AS_STRING(key));
}