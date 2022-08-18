#ifndef ERROR_H
#define ERROR_H

#include "common.h"

typedef enum ErrorType {
	ERROR_NONE,
	ERROR_SCAN_UNTERMINATED_STRING,
	ERROR_SCAN_UNTERMINATED_LIST,
} ErrorType;

typedef struct Error {
	ErrorType type;
	int line;
} Error;

typedef struct ErrorTracker {
	int errorsCapacity;
	int errorsSize;
	Error *errors;
} ErrorTracker;

ErrorTracker *error_tracker_create();
void error_tracker_destroy(ErrorTracker *errorTracker);

bool error_tracker_empty(ErrorTracker *errorTracker);
void error_tracker_add(ErrorTracker *errorTracker, ErrorType type, int line);

void error_tracker_print(ErrorTracker *errorTracker);

#endif