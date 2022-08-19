#ifndef EVAL_H
#define EVAL_H

#include "common.h"
#include "env.h"
#include "value/value.h"

Value *eval(Env *env, Value *value);

#endif