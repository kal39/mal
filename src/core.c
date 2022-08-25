#include "core.h"
#include "common.h"
#include "print.h"

static bool __equals(Value *a, Value *b) {
	if (TYPE(a) != TYPE(b)) return false;

	switch (TYPE(a)) {
		case VALUE_TYPE_NIL:
		case VALUE_TYPE_TRUE:
		case VALUE_TYPE_FALSE: return true;
		case VALUE_TYPE_PAIR: {
			int lenA = LEN(a);
			int lenB = LEN(b);
			if (lenA != lenB) return false;
			for (int i = 0; i < lenA; i++) {
				if (!__equals(FIRST(a), FIRST(b))) return false;
			}
			return true;
		}
		case VALUE_TYPE_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
		case VALUE_TYPE_SYMBOL:
		case VALUE_TYPE_ERROR:
		case VALUE_TYPE_STRING: {
			int lenA = strlen(AS_STRING(a));
			int lenB = strlen(AS_STRING(b));
			return lenA == lenB && memcmp(AS_STRING(a), AS_STRING(b), lenA) == 0 ? true : false;
		}
		case VALUE_TYPE_FUNCTION: return __equals(AS_FUNCTION(a).args, AS_FUNCTION(b).args) && __equals(AS_FUNCTION(a).body, AS_FUNCTION(b).body);
		case VALUE_TYPE_C_FUNCTION: return AS_C_FUNCTION(a) == AS_C_FUNCTION(b);
		default: return false;
	}
}

static Value *_add(Value *args) {
	double result = 0;
	ITERATE_LIST(i, args) {
		if (!IS_NUMBER(FIRST(i))) return MAKE_ERROR("expected number", args);
		result += AS_NUMBER(FIRST(i));
	}
	return MAKE_NUMBER(result);
}

static Value *_subtract(Value *args) {
	switch (LEN(args)) {
		case 0: return MAKE_ERROR("expected 1+ arguments", args);
		case 1:
			if (!IS_NUMBER(FIRST(args))) return MAKE_ERROR("expected number", args);
			else return MAKE_NUMBER(-AS_NUMBER(FIRST(args)));
		default: {
			double result = AS_NUMBER(FIRST(args));
			ITERATE_LIST(i, REST(args)) {
				if (!IS_NUMBER(FIRST(i))) return MAKE_ERROR("expected number", args);
				result -= AS_NUMBER(FIRST(i));
			}
			return MAKE_NUMBER(result);
		}
	}
}

static Value *_multiply(Value *args) {
	double result = 1;
	ITERATE_LIST(i, args) {
		if (!IS_NUMBER(FIRST(i))) return MAKE_ERROR("expected number", args);
		result *= AS_NUMBER(FIRST(i));
	}
	return MAKE_NUMBER(result);
}

static Value *_divide(Value *args) {
	switch (LEN(args)) {
		case 0: return MAKE_ERROR("expected 1+ arguments", args);
		case 1:
			if (!IS_NUMBER(FIRST(args))) return MAKE_ERROR("expected number", args);
			else return MAKE_NUMBER(1.0 / AS_NUMBER(FIRST(args)));
		default: {
			double result = AS_NUMBER(FIRST(args));
			ITERATE_LIST(i, REST(args)) {
				if (!IS_NUMBER(FIRST(i))) return MAKE_ERROR("expected number", args);
				result /= AS_NUMBER(FIRST(i));
			}
			return MAKE_NUMBER(result);
		}
	}
}

static Value *_print(Value *args) {
	if (LEN(args) < 1) return MAKE_ERROR("expected 1+ arguments", args);
	ITERATE_LIST(i, args) value_print(FIRST(i));
	return MAKE_NIL();
}

static Value *_cons(Value *args) {
	if (LEN(args) != 2) return MAKE_ERROR("expected 2 argument", args);
	return MAKE_PAIR(FIRST(args), SECOND(args));
}

static Value *_list(Value *args) {
	Value *list = MAKE_LIST();
	ITERATE_LIST(i, args) ADD_VALUE(list, FIRST(i));
	return list;
}

static Value *_is_list(Value *args) {
	if (LEN(args) != 1) return MAKE_ERROR("expected 1 argument", args);
	return IS_LIST(FIRST(args)) || IS_NIL(FIRST(args)) ? MAKE_TRUE() : MAKE_FALSE();
}

static Value *_is_empty(Value *args) {
	if (LEN(args) != 1) return MAKE_ERROR("expected 1 argument", args);
	return IS_NIL(FIRST(args)) ? MAKE_TRUE() : MAKE_FALSE();
}

static Value *_count(Value *args) {
	if (LEN(args) != 1) return MAKE_ERROR("expected 1 argument", args);
	if (IS_NIL(FIRST(args))) return MAKE_NUMBER(0);
	if (!IS_LIST(FIRST(args))) return MAKE_ERROR("expected list", args);
	return MAKE_NUMBER(LEN(FIRST(args)));
}

static Value *_equals(Value *args) {
	if (LEN(args) < 2) return MAKE_ERROR("expected 2+ argument", args);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (!__equals(FIRST(i), SECOND(i))) return MAKE_FALSE();
	}
	return MAKE_TRUE();
}

static Value *_less_than(Value *args) {
	if (LEN(args) < 2) return MAKE_ERROR("expected 2+ argument", args);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (AS_NUMBER(FIRST(i)) >= AS_NUMBER(SECOND(i))) return MAKE_FALSE();
	}
	return MAKE_TRUE();
}

static Value *_less_than_equals(Value *args) {
	if (LEN(args) < 2) return MAKE_ERROR("expected 2+ argument", args);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (AS_NUMBER(FIRST(i)) > AS_NUMBER(SECOND(i))) return MAKE_FALSE();
	}
	return MAKE_TRUE();
}

static Value *_greater_than(Value *args) {
	if (LEN(args) < 2) return MAKE_ERROR("expected 2+ argument", args);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (AS_NUMBER(FIRST(i)) <= AS_NUMBER(SECOND(i))) return MAKE_FALSE();
	}
	return MAKE_TRUE();
}

static Value *_greater_than_equals(Value *args) {
	if (LEN(args) < 2) return MAKE_ERROR("expected 2+ argument", args);
	for (Value *i = args; !IS_NIL(REST(i)); i = REST(i)) {
		if (AS_NUMBER(FIRST(i)) < AS_NUMBER(SECOND(i))) return MAKE_FALSE();
	}
	return MAKE_TRUE();
}

static Value *_not(Value *args) {
	if (LEN(args) != 1) return MAKE_ERROR("expected 1 argument", args);
	if (IS_TRUE(FIRST(args))) return MAKE_FALSE();
	if (IS_FALSE(FIRST(args))) return MAKE_TRUE();
	return MAKE_ERROR("expected boolean", args);
}

Env *make_core() {
	Env *core = env_create(NULL);
	env_set(core, MAKE_SYMBOL("+"), MAKE_C_FUNCTION(_add));
	env_set(core, MAKE_SYMBOL("-"), MAKE_C_FUNCTION(_subtract));
	env_set(core, MAKE_SYMBOL("*"), MAKE_C_FUNCTION(_multiply));
	env_set(core, MAKE_SYMBOL("/"), MAKE_C_FUNCTION(_divide));
	env_set(core, MAKE_SYMBOL("print"), MAKE_C_FUNCTION(_print));
	env_set(core, MAKE_SYMBOL("cons"), MAKE_C_FUNCTION(_cons));
	env_set(core, MAKE_SYMBOL("list"), MAKE_C_FUNCTION(_list));
	env_set(core, MAKE_SYMBOL("list?"), MAKE_C_FUNCTION(_is_list));
	env_set(core, MAKE_SYMBOL("empty?"), MAKE_C_FUNCTION(_is_empty));
	env_set(core, MAKE_SYMBOL("count"), MAKE_C_FUNCTION(_count));
	env_set(core, MAKE_SYMBOL("="), MAKE_C_FUNCTION(_equals));
	env_set(core, MAKE_SYMBOL("<"), MAKE_C_FUNCTION(_less_than));
	env_set(core, MAKE_SYMBOL("<="), MAKE_C_FUNCTION(_less_than_equals));
	env_set(core, MAKE_SYMBOL(">"), MAKE_C_FUNCTION(_greater_than));
	env_set(core, MAKE_SYMBOL(">="), MAKE_C_FUNCTION(_greater_than_equals));
	env_set(core, MAKE_SYMBOL("!"), MAKE_C_FUNCTION(_not));
	return core;
}