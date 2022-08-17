#include <stdio.h>

char *read(char *str) {
	return str;
}

char *eval(char *str) {
	return str;
}

char *print(char *str) {
	return str;
}

char *rep(char *str) {
	return str;
}

static void _repl() {
	char line[1024];
	for (;;) {
		printf(" > ");
		fgets(line, sizeof(line), stdin);
		printf("%s\n", rep(line));
	}
}

int main(void) {
	_repl();
	return 0;
}