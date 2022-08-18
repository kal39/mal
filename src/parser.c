#include "parser.h"
#include "error.h"

#define PEAK() reader_peak(reader)
#define NEXT() reader_next(reader)

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

static Value *_read_atom(Reader *reader, ErrorTracker *errorTracker) {
	Token token = NEXT();
	if (_match_content(token, "nil")) return MAKE_NIL();
	if (_match_content(token, "true")) return MAKE_TRUE();
	if (_match_content(token, "false")) return MAKE_FALSE();
	if (_is_number(token)) return MAKE_NUMBER(strtod(token.start, NULL));
	if (token.start[0] == '\"') return MAKE_STRING(token.start + 1, token.length - 2);
	else return MAKE_SYMBOL(token.start, token.length);
}

static Value *_read_list(Reader *reader, ErrorTracker *errorTracker) {
	NEXT();
	Value *list = MAKE_LIST();

	while (PEAK().start[0] != ')') {
		if (reader_at_end(reader)) {
			error_tracker_add(errorTracker, ERROR_SCAN_UNTERMINATED_LIST, 0); // TODO: line info
			break;
		}
		ADD_VALUE(list, read(reader, errorTracker));
	}

	NEXT();
	return list;
}

Value *read(Reader *reader, ErrorTracker *errorTracker) {
	if (reader->tokensSize == 0) return MAKE_NIL();
	return PEAK().start[0] == '(' ? _read_list(reader, errorTracker) : _read_atom(reader, errorTracker);
}