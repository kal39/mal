#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "common.h"
#include "value/table.h"

typedef struct Env Env;
typedef struct Env {
	Env *outer;
	Table *table;
} Env;

Env *env_create(Env *outer);
void env_destroy(Env *env);

void env_add(Env *env, Value *key, Value *value);
Value *env_get(Env *env, Value *key);

#endif