#include "error.h"

void _print_error(Error error) {
	printf("ERROR: ");

	switch (error.type) {
		case ERROR_SCAN_UNTERMINATED_STRING: printf("Unterminated string, missing a '\"'"); break;
		case ERROR_SCAN_UNTERMINATED_LIST: printf("Unterminated string, missing a ')'"); break;
		default: printf("Unknown error type"); break;
	}

	printf(" at line %d\n", error.line);
}

ErrorTracker *error_tracker_create() {
	ErrorTracker *errorTracker = malloc(sizeof(ErrorTracker));
	errorTracker->errorsCapacity = 8;
	errorTracker->errorsSize = 0;
	errorTracker->errors = malloc(sizeof(Error) * errorTracker->errorsCapacity);
	return errorTracker;
}

void error_tracker_destroy(ErrorTracker *errorTracker) {
	free(errorTracker->errors);
	free(errorTracker);
}

bool error_tracker_empty(ErrorTracker *errorTracker) {
	return errorTracker->errorsSize == 0;
}

void error_tracker_add(ErrorTracker *errorTracker, ErrorType type, int line) {
	if (errorTracker->errorsSize == errorTracker->errorsCapacity)
		errorTracker->errors = realloc(errorTracker->errors, sizeof(Error) * (errorTracker->errorsCapacity *= 2));
	errorTracker->errors[errorTracker->errorsSize++] = (Error){.type = type, .line = line};
}

void error_tracker_print(ErrorTracker *errorTracker) {
	for (int i = 0; i < errorTracker->errorsSize; i++) _print_error(errorTracker->errors[i]);
}