#ifndef READER_H
#define READER_H

#include "common.h"
#include "error.h"
#include "value.h"

typedef struct Token {
	char *start;
	int length;
} Token;

typedef struct Reader {
	bool atEnd;
	char *source;
	char *startChar;
	char *currentChar;

	int currentToken;
	int tokensCapacity;
	int tokensSize;
	Token *tokens;
} Reader;

Reader *reader_create();
void reader_destroy(Reader *reader);

void reader_scan(Reader *reader, char *source, ErrorTracker *errorTracker);

Token reader_peak(Reader *reader);
Token reader_next(Reader *reader);
bool reader_at_end(Reader *reader);

void reader_print(Reader *reader);

#endif