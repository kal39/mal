#include "core.h"

static Value *_add(Value *args) {
	if (list_length(args) == 0) return MAKE_ERROR("0 arguments provided", args);

	double result = 0;
	ITERATE_LIST(i, args) {
		if (!IS_NUMBER(FIRST(i))) return MAKE_ERROR("expected number", args);
		else result += AS_NUMBER(FIRST(i));
	}
	return MAKE_NUMBER(result);
}

static Value *_subtract(Value *args) {
	switch (list_length(args)) {
		case 0: return MAKE_ERROR("0 arguments provided", args);
		case 1: return MAKE_NUMBER(-AS_NUMBER(args));
		default: {
			double result = AS_NUMBER(FIRST(args));
			ITERATE_LIST(i, args) {
				if (!IS_NUMBER(FIRST(i))) return MAKE_ERROR("expected number", args);
				else result -= AS_NUMBER(FIRST(i));
			}
			return MAKE_NUMBER(result);
		}
	}
}

static Value *_multiply(Value *args) {
	if (list_length(args) == 0) return MAKE_ERROR("0 arguments provided", args);

	double result = 1;
	ITERATE_LIST(i, args) {
		if (!IS_NUMBER(FIRST(i))) return MAKE_ERROR("expected number", args);
		else result *= AS_NUMBER(FIRST(i));
	}
	return MAKE_NUMBER(result);
}

static Value *_divide(Value *args) {
	switch (list_length(args)) {
		case 0: return MAKE_ERROR("0 arguments provided", args);
		case 1: return MAKE_NUMBER(1.0 / AS_NUMBER(args));
		default: {
			double result = AS_NUMBER(FIRST(args));
			ITERATE_LIST(i, args) {
				if (!IS_NUMBER(FIRST(i))) return MAKE_ERROR("expected number", args);
				else result /= AS_NUMBER(FIRST(i));
			}
			return MAKE_NUMBER(result);
		}
	}
}

Env *make_core() {
	Env *core = env_create(NULL);
	env_set(core, MAKE_SYMBOL("+"), MAKE_C_FUNCTION(_add));
	env_set(core, MAKE_SYMBOL("-"), MAKE_C_FUNCTION(_subtract));
	env_set(core, MAKE_SYMBOL("*"), MAKE_C_FUNCTION(_multiply));
	env_set(core, MAKE_SYMBOL("/"), MAKE_C_FUNCTION(_divide));
	return core;
}