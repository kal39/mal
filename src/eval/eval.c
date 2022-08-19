#include "eval.h"

static Value *_eval(Env *env, Value *ast, int depth);

static Value *_apply(Env *env, Value *ast, int depth) {
	switch (TYPE(ast)) {
		case VALUE_TYPE_PAIR: {
			Value *evaluatedList = _eval(env, ast, depth);
			return AS_C_FUNCTION(FIRST(evaluatedList))(REST(evaluatedList));
		}
		case VALUE_TYPE_NIL: return ast;
		default: return _eval(env, ast, depth);
	}
}

static Value *_eval(Env *env, Value *ast, int depth) {
	switch (TYPE(ast)) {
		case VALUE_TYPE_PAIR: {
			Value *appliedList = list_create();
			ITERATE_LIST(iterator, ast)
			list_add_value(appliedList, _apply(env, FIRST(iterator), depth));
			return appliedList;
		}
		case VALUE_TYPE_SYMBOL: return env_get(env, ast); break;
		default: return ast;
	}
}

// static Value *_eval(Env *env, Value *value, int depth) {
// #ifdef PRINT_EVALUATION_STEPS
// 	for (int i = 0; i < depth * PRINT_INDENT_SIZE; i++) printf(" ");
// 	value_print(value);
// 	printf("\n");
// #endif

// 	Value *result = value;
// 	switch (TYPE(value)) {
// 		case VALUE_TYPE_PAIR: {
// 			Value *evaluatedList = list_create();
// 			ITERATE_LIST(iterator, value)
// 			list_add_value(evaluatedList, _eval(env, FIRST(iterator), depth + 1));

// 			if (!IS_C_FUNCTION(FIRST(evaluatedList))) result = MAKE_ERROR("not a function", value);
// 			else result = AS_C_FUNCTION(FIRST(evaluatedList))(evaluatedList);
// 			break;
// 		}
// 		case VALUE_TYPE_SYMBOL: result = env_get(env, value); break;
// 		default: break;
// 	}

// #ifdef PRINT_EVALUATION_STEPS
// 	for (int i = 0; i < depth * PRINT_INDENT_SIZE; i++) printf(" ");
// 	printf("= ");
// 	value_print_depth(result, depth);
// 	printf("\n");
// #endif

// 	return result;
// }

Value *eval(Env *env, Value *ast) {
	return _eval(env, ast, 0);
}