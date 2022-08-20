#include "reader.h"

static void _add_token(Tokenizer *tokenizer, Token token) {
	if (tokenizer->tokensSize == tokenizer->tokensCapacity)
		tokenizer->tokens = realloc(tokenizer->tokens, sizeof(Token) * (tokenizer->tokensCapacity *= 2));
	tokenizer->tokens[tokenizer->tokensSize++] = token;
}

static void _token(Tokenizer *tokenizer) {
	_add_token(tokenizer,
			   (Token){.start = tokenizer->startChar, .length = tokenizer->currentChar - tokenizer->startChar});
	tokenizer->startChar = tokenizer->currentChar;
}

static char _next(Tokenizer *tokenizer) {
	return *tokenizer->currentChar++;
}

static char _peek(Tokenizer *tokenizer) {
	return *tokenizer->currentChar;
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

static bool _at_end(Tokenizer *tokenizer) {
	if (_peek(tokenizer) == '\0') tokenizer->atEnd = true;
	return tokenizer->atEnd;
}

static bool _read_string(Tokenizer *tokenizer) {
	int line = 0;
	bool stringNotTerminated = false;

	while (!_at_end(tokenizer)) {
		char c = _next(tokenizer);

		if (_is_whitespace(c, &line)) { // skip whitespace
			tokenizer->startChar = tokenizer->currentChar;
		} else if (_is_special(c)) { // special characters
			_token(tokenizer);
		} else if (c == '"') { // strings
			while (_peek(tokenizer) != '"') {
				if (_at_end(tokenizer)) {
					stringNotTerminated = true;
					break;
				}
				_next(tokenizer);
				if (_peek(tokenizer) == '\\') _next(tokenizer);
			}
			_next(tokenizer);
			_token(tokenizer);
		} else if (c == ';') { // comments
			while (!_is_whitespace(_peek(tokenizer), &line) && !_at_end(tokenizer)) _next(tokenizer);
			_next(tokenizer);
			tokenizer->startChar = tokenizer->currentChar;
		} else { // symbols or numbers
			while (!_is_whitespace(_peek(tokenizer), &line) && !_is_special(_peek(tokenizer)) && !_at_end(tokenizer))
				_next(tokenizer);
			_token(tokenizer);
		}
	}

	_add_token(tokenizer, (Token){.length = -1});
	return stringNotTerminated;
}

Tokenizer *tokenizer_create() {
	return malloc(sizeof(Tokenizer));
}

bool tokenizer_scan(Tokenizer *tokenizer, char *source) {
	tokenizer->atEnd = false;
	tokenizer->source = source;
	tokenizer->startChar = tokenizer->source;
	tokenizer->currentChar = tokenizer->source;

	tokenizer->currentToken = 0;
	tokenizer->tokensCapacity = 8;
	tokenizer->tokensSize = 0;
	tokenizer->tokens = malloc(sizeof(Token) * tokenizer->tokensCapacity);

	return _read_string(tokenizer);
}

void tokenizer_destroy(Tokenizer *tokenizer) {
	free(tokenizer->tokens);
	free(tokenizer);
}

Token tokenizer_peak(Tokenizer *tokenizer) {
	return tokenizer->tokens[tokenizer->currentToken];
}

Token tokenizer_next(Tokenizer *tokenizer) {
	if (tokenizer->currentToken < tokenizer->tokensSize - 1) return tokenizer->tokens[tokenizer->currentToken++];
	else return tokenizer->tokens[tokenizer->currentToken];
}

bool tokenizer_current_end(Tokenizer *tokenizer) {
	return tokenizer->currentToken == tokenizer->tokensSize - 1;
}

bool tokenizer_next_end(Tokenizer *tokenizer) {
	return tokenizer->currentToken == tokenizer->tokensSize;
}

void tokenizer_print(Tokenizer *tokenizer) {
	for (int i = 0; i < tokenizer->tokensSize; i++) {
		if (IS_END_TOKEN(tokenizer->tokens[i])) printf("END");
		else printf("%.*s", tokenizer->tokens[i].length, tokenizer->tokens[i].start);
		if (i != tokenizer->tokensSize - 1) printf(" · ");
	}
}