#include "eval.h"
#include "common.h"
#include "print.h"

#define EVAL(env, ast) _eval(env, ast, depth + 1, line)
static Value *_eval(Env *env, Value *ast, int depth, int *line);

static Value *__eval(Env *env, Value *ast, int depth, int *line) {
	switch (TYPE(ast)) {
		case VALUE_TYPE_PAIR: {
			switch (TYPE(FIRST(ast))) {
				case VALUE_TYPE_DEF: {
					if (LEN(REST(ast)) != 2) return MAKE_ERROR("expected 2 arguments", REST(ast));
					Value *key = SECOND(ast);
					Value *value = EVAL(env, THIRD(ast));
					env_set(env, key, value);
					return value;
				}

				case VALUE_TYPE_LET: {
					if (LEN(REST(ast)) != 2) return MAKE_ERROR("expected 2 arguments", REST(ast));
					if (!IS_LIST(FIRST(SECOND(ast)))) return MAKE_ERROR("expected list", FIRST(SECOND(ast)));

					Env *newEnv = env_create(env);
					ITERATE_LIST(i, SECOND(ast)) {
						if (LEN(FIRST(i)) != 2) return MAKE_ERROR("expected 2 arguments", FIRST(i));
						env_set(env, FIRST(FIRST(i)), EVAL(env, SECOND(FIRST(i))));
					}
					Value *result = EVAL(newEnv, THIRD(ast));
					env_destroy(newEnv);
					return result;
				}

				case VALUE_TYPE_DO: {
					Value *results = MAKE_LIST();
					ITERATE_LIST(i, REST(ast)) ADD_VALUE(results, EVAL(env, FIRST(i)));
					return LAST(results);
				}

				case VALUE_TYPE_IF: {
					if (LEN(REST(ast)) != 3) return MAKE_ERROR("expected 3 arguments", REST(ast));
					Value *result = EVAL(env, SECOND(ast));
					return EVAL(env, IS_NIL(result) || IS_FALSE(result) ? FOURTH(ast) : THIRD(ast));
				}

				case VALUE_TYPE_FN: {
					if (LEN(REST(ast)) != 2) return MAKE_ERROR("expected 2 arguments", REST(ast));
					else return MAKE_FUNCTION(env_create(env), SECOND(ast), THIRD(ast));
				}

				case VALUE_TYPE_SYMBOL: {
					Value *evaluatedAst = MAKE_LIST();
					ITERATE_LIST(i, ast) ADD_VALUE(evaluatedAst, EVAL(env, FIRST(i)));

					switch (TYPE(FIRST(evaluatedAst))) {
						case VALUE_TYPE_FUNCTION: {
							Value *function = FIRST(evaluatedAst);
							Value *argNames = AS_FUNCTION(function).args;
							Value *argValues = REST(evaluatedAst);
							int argNamesLen = LEN(argNames);
							int argValuesLen = LEN(argValues);

							if (argNamesLen != argValuesLen) return MAKE_ERROR("incorrect argument count", argValues);

							Env *funEnv = env_create(AS_FUNCTION(function).outer);

							for (int i = 0; i < LEN(argValues) + 1; i++) {
								env_set(funEnv, FIRST(argNames), FIRST(argValues));
								argNames = REST(argNames);
								argValues = REST(argValues);
							}

							return EVAL(funEnv, AS_FUNCTION(function).body);
						}

						case VALUE_TYPE_C_FUNCTION: return AS_C_FUNCTION(FIRST(evaluatedAst))(REST(evaluatedAst));
						default: return MAKE_ERROR("expected function", FIRST(evaluatedAst));
					}
				}

				default: return MAKE_ERROR("expected function", FIRST(ast));
			}
		}
		case VALUE_TYPE_SYMBOL: return env_get(env, ast);
		default: return ast;
	}
}

static Value *_eval(Env *env, Value *ast, int depth, int *line) {
#ifdef PRINT_EVALUATION_STEPS
	*line = *line + 1;
	int startLine = *line;

	printf("\n");
	for (int i = 0; i < depth; i++) printf(" ║ ");
	value_print_debug_offset(ast, depth * 3);
#endif

	Value *ret = __eval(env, ast, depth, line);

#ifdef PRINT_EVALUATION_STEPS
	if (*line == startLine) {
		printf(" ═ ");
		value_print_debug_offset(ret, depth * 4 + 2);
	} else {
		*line = *line + 1;
		printf("\n");
		for (int i = 0; i < depth; i++) printf(" ║ ");
		printf(" ╚═ ");
		value_print_debug_offset(ret, depth * 3 + 4);
	}
#endif

	return ret;
}

Value *eval(Env *env, Value *ast) {
	int line = 0;
	Value *result = _eval(env, ast, 0, &line);
#ifdef PRINT_EVALUATION_STEPS
	printf("\n");
#endif
	return result;
}
