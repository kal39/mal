#include "common.h"
#include "error.h"
#include "parser.h"
#include "reader.h"
#include "value.h"

/*
TODO: Keywords
TODO: Vectors
TODO: Hashmaps
*/

static void _rep(char *string) {
	ErrorTracker *errorTracker = error_tracker_create();
	Reader *reader = reader_create();

	reader_scan(reader, string, errorTracker);
	if (!error_tracker_empty(errorTracker)) {
		error_tracker_print(errorTracker);
		return;
	}

	reader_print(reader);

	Value *root = read(reader, errorTracker);
	if (!error_tracker_empty(errorTracker)) {
		error_tracker_print(errorTracker);
		return;
	}

	value_print(root, false);
	printf("\n");

	reader_destroy(reader);
	error_tracker_destroy(errorTracker);
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