#include "parser.h"

#define PEEK() scanner_peak(scanner)
#define NEXT() scanner_next(scanner)

static bool _is_digit(char c) {
	return c >= '0' && c <= '9';
}

static bool _is_number(Token token) {
	if (!_is_digit(token.start[0])) return false;
	for (int i = 1; i < token.length; i++) {
		if (!_is_digit(token.start[i]) && token.start[i] != '.') return false;
		if (token.start[i] == '.' && !_is_digit(token.start[i + 1])) return false;
	}
	return true;
}

static bool _match_content(Token token, char *content) {
	return token.length == strlen(content) && memcmp(token.start, content, token.length) == 0;
}

static Value *_read_atom(Scanner *scanner) {
	Token token = NEXT();
	if (_match_content(token, "nil")) return MAKE_NIL();
	if (_match_content(token, "true")) return MAKE_TRUE();
	if (_match_content(token, "false")) return MAKE_FALSE();
	if (_is_number(token)) return MAKE_NUMBER(strtod(token.start, NULL));
	if (token.start[0] == '\"') return MAKE_STRING_LEN(token.start + 1, token.length - 2);
	else return MAKE_SYMBOL_LEN(token.start, token.length);
}

static Value *_parse(Scanner *scanner);

static Value *_read_list(Scanner *scanner) {
	NEXT();
	Value *list = list_create();

	for (;;) {
		if (IS_END_TOKEN(PEEK())) return MAKE_ERROR("unterminated list, missing ')", list);
		if (PEEK().start[0] == ')') break;
		list_add_value(list, _parse(scanner));
	}

	NEXT();
	return list;
}

static Value *_parse(Scanner *scanner) {
	return PEEK().start[0] == '(' ? _read_list(scanner) : _read_atom(scanner);
}

Value *parse(Scanner *scanner) {
	Value *ast = list_create();
	while (!IS_END_TOKEN(PEEK())) {
		list_add_value(ast, _parse(scanner));
	}
	return ast;
}