#include "eval.h"

static Value *_eval_ast(Env *env, Value *ast);

static Value *_bind(Env *env, Value *list) {
	if (list_length(list) != 2) return MAKE_ERROR("expected 2 arguments", list);
	Value *key = FIRST(list);
	Value *value = _eval_ast(env, FIRST(REST(list)));
	env_set(env, key, value);
	return value;
}

static Value *_apply(Env *env, Value *ast) {
	switch (TYPE(ast)) {
		case VALUE_TYPE_PAIR: {
			if (IS_SYMBOL(FIRST(ast))) {
				if (STRING_EQUALS(AS_SYMBOL(FIRST(ast)), "def")) {
					return _bind(env, REST(ast));

				} else if (STRING_EQUALS(AS_SYMBOL(FIRST(ast)), "let")) {
					if (list_length(REST(ast)) < 2) return MAKE_ERROR("expected 2+ arguments", REST(ast));
					if (!IS_LIST(FIRST(FIRST(REST(ast))))) return MAKE_ERROR("expected list", FIRST(FIRST(REST(ast))));

					Env *newEnv = env_create(env);
					ITERATE_LIST(iterator, FIRST(REST(ast))) _bind(newEnv, FIRST(iterator));
					Value *result = eval(newEnv, REST(REST(ast)));
					env_destroy(newEnv);
					return result;
				}
			}

			Value *evaluatedAst = _eval_ast(env, ast);
			if (!IS_C_FUNCTION(FIRST(evaluatedAst))) return MAKE_ERROR("expected function", FIRST(evaluatedAst));
			else return AS_C_FUNCTION(FIRST(evaluatedAst))(REST(evaluatedAst));
		}
		case VALUE_TYPE_NIL: return ast;
		default: return _eval_ast(env, ast);
	}
}

static Value *_eval_ast(Env *env, Value *ast) {
	switch (TYPE(ast)) {
		case VALUE_TYPE_PAIR: {
			Value *appliedList = list_create();
			ITERATE_LIST(iterator, ast) list_add_value(appliedList, _apply(env, FIRST(iterator)));
			return appliedList;
		}
		case VALUE_TYPE_SYMBOL: return env_get(env, ast); break;
		default: return ast;
	}
}

Value *eval(Env *env, Value *ast) {
	return list_last(_eval_ast(env, ast));
	// return _eval_ast(env, ast);
}