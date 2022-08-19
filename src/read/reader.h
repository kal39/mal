#ifndef READER_H
#define READER_H

#include "common.h"
#include "value/value.h"

typedef struct Token {
	char *start;
	int length;
} Token;

typedef struct Tokenizer {
	bool atEnd;
	char *source;
	char *startChar;
	char *currentChar;

	int currentToken;
	int tokensCapacity;
	int tokensSize;
	Token *tokens;
} Tokenizer;

Tokenizer *tokenizer_create();
void tokenizer_destroy(Tokenizer *tokenizer);

bool tokenizer_scan(Tokenizer *tokenizer, char *source);

Token tokenizer_peak(Tokenizer *tokenizer);
Token tokenizer_next(Tokenizer *tokenizer);
bool tokenizer_at_end(Tokenizer *tokenizer);

void tokenizer_print(Tokenizer *tokenizer);

#endif