#include "common.h"
#include "eval/core.h"
#include "eval/env.h"
#include "eval/eval.h"
#include "read/parser.h"
#include "read/reader.h"
#include "value/value.h"

/*
TODO: keywords
TODO: vectors
TODO: hashmaps

TODO: eval vectors
TODO: eval hashmaps
*/

static Value *_read(char *string) {
	Tokenizer *tokenizer = tokenizer_create();
	if (tokenizer_scan(tokenizer, string)) {
		return MAKE_ERROR("Unterminated string", MAKE_NIL());
	} else {
		Value *ast = read(tokenizer);
		tokenizer_destroy(tokenizer);
		return ast;
	}
}

static Value *_eval(Value *ast) {
	return eval(make_core(), ast);
}

static void _print(Value *result) {
	printf("\n = ");
	value_print(result);
	printf("\n");
}

static void _rep(char *string) {
	_print(_eval(_read(string)));
}

static void _repl() {
	char line[1024];
	for (;;) {
		printf(" > ");
		fgets(line, sizeof(line), stdin);
		_rep(line);
	}
}

int main(void) {
	_repl();
	return 0;
}