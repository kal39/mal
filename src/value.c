#include "value.h"

Value *value_create(ValueType type) {
	Value *value = malloc(sizeof(Value));
	value->type = type;
	return value;
}

Value *value_create_pair(Value *first, Value *second) {
	Value *value = value_create(VALUE_TYPE_PAIR);
	value->as.pair.first = first;
	value->as.pair.second = second;
	return value;
}

Value *value_create_symbol(char *symbol, int symbolLength) {
	Value *value = value_create(VALUE_TYPE_SYMBOL);
	value->as.symbol = malloc(symbolLength + 1);
	memcpy(value->as.symbol, symbol, symbolLength);
	value->as.symbol[symbolLength] = '\0';
	return value;
}

Value *value_create_number(double number) {
	Value *value = value_create(VALUE_TYPE_NUMBER);
	value->as.number = number;
	return value;
}

Value *value_create_string(char *string, int stringLength) {
	Value *value = value_create(VALUE_TYPE_STRING);
	value->as.string = malloc(stringLength + 1);

	int i;
	int skippedChars = 0;
	for (i = 0; i < stringLength; i++) {
		if (string[i] == '\\') {
			skippedChars++;
			i++;
			switch (string[i]) {
				case 'a': value->as.string[i - skippedChars] = '\a'; break;
				case 'b': value->as.string[i - skippedChars] = '\b'; break;
				case 'e': value->as.string[i - skippedChars] = '\e'; break;
				case 'f': value->as.string[i - skippedChars] = '\f'; break;
				case 'n': value->as.string[i - skippedChars] = '\n'; break;
				case 'r': value->as.string[i - skippedChars] = '\r'; break;
				case 't': value->as.string[i - skippedChars] = '\t'; break;
				case '\\': value->as.string[i - skippedChars] = '\\'; break;
				case '\'': value->as.string[i - skippedChars] = '\''; break;
				case '"': value->as.string[i - skippedChars] = '"'; break;
				case '?': value->as.string[i - skippedChars] = '\?'; break;
				default: break;
			}
		} else {
			value->as.string[i - skippedChars] = string[i];
		}
	}

	value->as.string[i - skippedChars] = '\0';
	return value;
}

Value *value_create_list() {
	return MAKE_NIL();
}

void value_add_item_to_list(Value *list, Value *value) {
	if (IS_NIL(list)) {
		list->type = VALUE_TYPE_PAIR;
		list->as.pair.first = value;
		list->as.pair.second = MAKE_NIL();
	} else if (IS_PAIR(list)) {
		if (IS_NIL(list->as.pair.second)) {
			list->as.pair.second->type = VALUE_TYPE_PAIR;
			list->as.pair.second->as.pair.first = value;
			list->as.pair.second->as.pair.second = MAKE_NIL();
		} else {
			value_add_item_to_list(list->as.pair.second, value);
		}
	}
}

void value_destroy(Value *value) {
	if (value == NULL) return;
	switch (value->type) {
		case VALUE_TYPE_PAIR:
			value_destroy(value->as.pair.first);
			value_destroy(value->as.pair.second);
			break;
		case VALUE_TYPE_SYMBOL: free(value->as.symbol); break;
		case VALUE_TYPE_STRING: free(value->as.string); break;
		default: break;
	}
	free(value);
}

void value_print(Value *value, bool listAsPairs) {
	if (value == NULL) {
		printf("!null");
		return;
	}
	switch (value->type) {
		case VALUE_TYPE_NIL: printf("nil"); break;
		case VALUE_TYPE_TRUE: printf("true"); break;
		case VALUE_TYPE_FALSE: printf("false"); break;
		case VALUE_TYPE_PAIR:
			if (listAsPairs) {
				printf("( ");
				value_print(value->as.pair.first, listAsPairs);
				printf(" . ");
				value_print(value->as.pair.second, listAsPairs);
				printf(" )");
			} else {
				printf("(");
				for (Value *next = value; !IS_NIL(next); next = next->as.pair.second) {
					value_print(next->as.pair.first, listAsPairs);
					printf(!IS_NIL(next->as.pair.second) ? " " : ")");
				}
			}
			break;
		case VALUE_TYPE_SYMBOL: printf("'%s'", value->as.symbol); break;
		case VALUE_TYPE_NUMBER: printf("%f", value->as.number); break;
		case VALUE_TYPE_STRING: printf("\"%s\"", value->as.string); break;
		default: break;
	}
}