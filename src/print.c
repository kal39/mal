#include "print.h"
#include "common.h"

bool _value_print(Value *value, int depth, int offset, bool inList, bool debugMode);

void _print_error(Value *value, int depth, int offset, bool debugMode) {
	printf("ERROR: %s at: ", AS_ERROR(value).string);
	_value_print(AS_ERROR(value).value, depth + 1, offset, false, debugMode);
}

// TODO: handle pairs
bool _value_print(Value *value, int depth, int offset, bool inList, bool debugMode) {
	if (value == NULL) {
		printf("NULL");
		return 0;
	}

	switch (TYPE(value)) {
		case VALUE_TYPE_NIL: printf("nil"); return 0;
		case VALUE_TYPE_TRUE: printf("true"); return 0;
		case VALUE_TYPE_FALSE: printf("false"); return 0;
		case VALUE_TYPE_DEF: printf("def"); return 0;
		case VALUE_TYPE_LET: printf("let"); return 0;
		case VALUE_TYPE_DO: printf("do"); return 0;
		case VALUE_TYPE_IF: printf("if"); return 0;
		case VALUE_TYPE_FN: printf("fn"); return 0;
		case VALUE_TYPE_PAIR: {
			bool hadError = false;
			printf("(");
			ITERATE_LIST(i, value) {
				if (_value_print(FIRST(i), depth + 1, offset, true, debugMode)) hadError = true;
				printf(!IS_NIL(REST(i)) ? " " : ")");
			}
			if (hadError) {
				ITERATE_LIST(i, value) {
					if (IS_ERROR(FIRST(i))) {
						printf("\n");
						for (int i = 0; i < depth * PRINT_INDENT_SIZE + offset; i++) printf(" ");
						_print_error(FIRST(i), depth, offset, debugMode);
					}
				}
			}
			return 0;
		}
		case VALUE_TYPE_SYMBOL:
			if (debugMode) printf("'%s'", AS_SYMBOL(value));
			else printf("%s", AS_SYMBOL(value));
			return 0;
		case VALUE_TYPE_NUMBER: printf("%g", AS_NUMBER(value)); return 0;
		case VALUE_TYPE_STRING:
			if (debugMode) printf("\"%s\"", AS_STRING(value));
			else printf("%s", AS_STRING(value));
			return 0;
		case VALUE_TYPE_FUNCTION: printf("FUNCTION"); return 0;
		case VALUE_TYPE_C_FUNCTION: printf("C_FUNCTION"); return 0;
		case VALUE_TYPE_ERROR:
			if (inList) printf("ERROR");
			else _print_error(value, depth, offset, debugMode);
			return 1;
		default: printf("UNKNOW"); return 0;
	}
}

void value_print(Value *value) {
	_value_print(value, 0, 0, false, false);
}

void value_print_debug(Value *value) {
	value_print_debug_offset(value, 0);
}

void value_print_debug_offset(Value *value, int offset) {
	_value_print(value, 0, offset, false, true);
}