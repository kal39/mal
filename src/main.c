#include "common.h"
#include "core.h"
#include "env.h"
#include "eval.h"
#include "parser.h"
#include "print.h"
#include "scanner.h"
#include "value.h"

/*
TODO: keywords
TODO: vectors
TODO: hashmaps
TODO: variadic func parameters
*/

static Value *_read_string(char *string) {
	Scanner *scanner = scanner_create(string);
	if (scanner == NULL) return MAKE_ERROR("Unterminated string", NULL, NULL);

	Value *asts = parse(scanner);

	scanner_destroy(scanner);
	return asts;
}

static void _rep(Env *core, char *string) {
	Value *asts = _read_string(string);

	ITERATE_LIST(i, asts) {
		Value *result = eval(core, FIRST(i));
		if (IS_ERROR(result)) {
			printf("\n");
			error_print(result);
		} else {
			printf("\n = ");
			value_print(result);
		}
		printf("\n");
	}
}

static void _repl() {
	Env *core = make_core();

	char line[1024];
	for (;;) {
		printf(" > ");
		fgets(line, sizeof(line), stdin);
		_rep(core, line);
	}

	env_destroy(core);
}

int main(void) {
	_repl();
	return 0;
}