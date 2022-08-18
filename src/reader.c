#include "reader.h"
#include "error.h"
#include "memory.h"

static void _token(Reader *reader) {
	if (reader->tokensSize == reader->tokensCapacity)
		reader->tokens = realloc(reader->tokens, sizeof(Token) * (reader->tokensCapacity *= 2));
	reader->tokens[reader->tokensSize++]
		= (Token){.start = reader->startChar, .length = reader->currentChar - reader->startChar};
	reader->startChar = reader->currentChar;
}

static char _next(Reader *reader) {
	return *reader->currentChar++;
}

static char _peek(Reader *reader) {
	return *reader->currentChar;
}

static bool _is_whitespace(char c, int *line) {
	switch (c) {
		case '\n': *line += 1;
		case ' ':
		case '\t':
		case '\v':
		case '\f':
		case '\r':
		case ',': return true;
		default: return false;
	}
}

static bool _is_special(char c) {
	switch (c) {
		case '[':
		case ']':
		case '{':
		case '}':
		case '(':
		case ')':
		case '\'':
		case '`':
		case '^':
		case '@': return true;
		default: return false;
	}
}

static bool _at_end(Reader *reader) {
	if (_peek(reader) == '\0') reader->atEnd = true;
	return reader->atEnd;
}

static void _read_string(Reader *reader, ErrorTracker *errorTracker) {
	int line = 0;
	bool stringNotTerminated = false;

	while (!_at_end(reader)) {
		char c = _next(reader);

		if (_is_whitespace(c, &line)) { // skip whitespace
			reader->startChar = reader->currentChar;
		} else if (_is_special(c)) { // special characters
			_token(reader);
		} else if (c == '"') { // strings
			while (_peek(reader) != '"') {
				if (_at_end(reader)) {
					stringNotTerminated = true;
					break;
				}
				_next(reader);
				if (_peek(reader) == '\\') _next(reader);
			}
			_next(reader);
			_token(reader);
		} else if (c == ';') { // comments
			while (!_is_whitespace(_peek(reader), &line) && !_at_end(reader)) _next(reader);
			_next(reader);
			reader->startChar = reader->currentChar;
		} else { // symbols or numbers
			while (!_is_whitespace(_peek(reader), &line) && !_is_special(_peek(reader)) && !_at_end(reader))
				_next(reader);
			_token(reader);
		}
	}

	if (stringNotTerminated) error_tracker_add(errorTracker, ERROR_SCAN_UNTERMINATED_STRING, line);
}

Reader *reader_create() {
	return malloc(sizeof(Reader));
}

void reader_scan(Reader *reader, char *source, ErrorTracker *errorTracker) {
	reader->atEnd = false;
	reader->source = source;
	reader->startChar = reader->source;
	reader->currentChar = reader->source;

	reader->currentToken = 0;
	reader->tokensCapacity = 8;
	reader->tokensSize = 0;
	reader->tokens = malloc(sizeof(Token) * reader->tokensCapacity);

	_read_string(reader, errorTracker);
}

void reader_destroy(Reader *reader) {
	free(reader->tokens);
	free(reader);
}

Token reader_peak(Reader *reader) {
	return reader->tokens[reader->currentToken];
}

Token reader_next(Reader *reader) {
	if (reader->currentToken < reader->tokensSize - 1) return reader->tokens[reader->currentToken++];
	else return reader->tokens[reader->currentToken];
}

bool reader_at_end(Reader *reader) {
	return reader->currentToken == reader->tokensSize - 1;
}

void reader_print(Reader *reader) {
	for (int i = 0; i < reader->tokensSize; i++) {
		printf("%.*s", reader->tokens[i].length, reader->tokens[i].start);
		printf(i != reader->tokensSize - 1 ? " · " : "\n");
	}
}