#include "parser.h"

#define PEAK() tokenizer_peak(tokenizer)
#define NEXT() tokenizer_next(tokenizer)

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

static Value *_read_atom(Tokenizer *tokenizer) {
	Token token = NEXT();
	if (_match_content(token, "nil")) return MAKE_NIL();
	if (_match_content(token, "true")) return MAKE_TRUE();
	if (_match_content(token, "false")) return MAKE_FALSE();
	if (_is_number(token)) return MAKE_NUMBER(strtod(token.start, NULL));
	if (token.start[0] == '\"') return MAKE_STRING_LEN(token.start + 1, token.length - 2);
	else return MAKE_SYMBOL_LEN(token.start, token.length);
}

static Value *_read(Tokenizer *tokenizer);

static Value *_read_list(Tokenizer *tokenizer) {
	NEXT();
	Value *list = list_create();

	while (PEAK().start[0] != ')') {
		if (tokenizer_at_end(tokenizer)) return MAKE_ERROR("unterminated list, missing ')", list);
		list_add_value(list, _read(tokenizer));
	}

	NEXT();
	return list;
}

static Value *_read(Tokenizer *tokenizer) {
	return PEAK().start[0] == '(' ? _read_list(tokenizer) : _read_atom(tokenizer);
}

Value *read(Tokenizer *tokenizer) {
	if (tokenizer->tokensSize == 0) return MAKE_NIL();
	Value *ast = list_create();
	while (!tokenizer_at_end(tokenizer)) list_add_value(ast, _read(tokenizer));
	return ast;
}