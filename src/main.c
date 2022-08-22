#include "common.h"
#include "eval/core.h"
#include "eval/env.h"
#include "eval/eval.h"
#include "read/parser.h"
#include "read/scanner.h"
#include "value/value.h"

/*
TODO: keywords
TODO: vectors
TODO: hashmaps

TODO: eval vectors
TODO: eval hashmaps
*/

static Value *_read_string(char *string) {
	Scanner *scanner = scanner_create(string);
	if (scanner == NULL) return MAKE_ERROR("Unterminated string", MAKE_NIL());

	// printf("   tokens:\n     ");
	// scanner_print(scanner);
	// printf("\n");

	Value *asts = parse(scanner);

	// printf("   ASTs:\n     ");
	// ITERATE_LIST(iterator, asts) {
	// 	value_print(FIRST(iterator));
	// 	printf("\n     ");
	// }

	scanner_destroy(scanner);
	return asts;
}

static void _rep(Env *core, char *string) {
	Value *asts = _read_string(string);

	ITERATE_LIST(iterator, asts) {
		printf("\n = ");
		value_print_offset(eval(core, FIRST(iterator)), 3);
		printf("\n");
	}

	// value_destroy(asts);
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