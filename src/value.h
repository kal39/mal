#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include "common.h"

typedef enum {
	VALUE_TYPE_NIL,
	VALUE_TYPE_TRUE,
	VALUE_TYPE_FALSE,
	VALUE_TYPE_PAIR,
	VALUE_TYPE_SYMBOL,
	VALUE_TYPE_NUMBER,
	VALUE_TYPE_STRING,
} ValueType;

typedef struct Value Value;

typedef struct Pair {
	Value *first;
	Value *second;
} Pair;

typedef struct Value {
	ValueType type;
	union {
		Pair pair;
		char *symbol;
		double number;
		char *string;
	} as;
} Value;

#define MAKE_NIL() (value_create(VALUE_TYPE_NIL))
#define MAKE_TRUE() (value_create(VALUE_TYPE_TRUE))
#define MAKE_FALSE() (value_create(VALUE_TYPE_FALSE))
#define MAKE_PAIR(first, second) (value_create_pair((first), (second)))
#define MAKE_SYMBOL(symbol, symbolLength) (value_create_symbol((symbol), (symbolLength)))
#define MAKE_NUMBER(number) (value_create_number((number)))
#define MAKE_STRING(string, stringLength) (value_create_string((string), (stringLength)))

#define IS_NIL(value) ((value)->type == VALUE_TYPE_NIL)
#define IS_TRUE(value) ((value)->type == VALUE_TYPE_TRUE)
#define IS_FALSE(value) ((value)->type == VALUE_TYPE_FALSE)
#define IS_PAIR(value) ((value)->type == VALUE_TYPE_PAIR)
#define IS_SYMBOL(value) ((value)->type == VALUE_TYPE_SYMBOL)
#define IS_NUMBER(value) ((value)->type == VALUE_TYPE_NUMBER)

#define MAKE_LIST() (value_create_list())
#define ADD_VALUE(list, value) (value_add_item_to_list((list), (value)))

Value *value_create(ValueType type);

Value *value_create_pair(Value *first, Value *second);
Value *value_create_symbol(char *symbol, int symbolLength);
Value *value_create_number(double number);
Value *value_create_string(char *string, int stringLength);

Value *value_create_list();
void value_add_item_to_list(Value *list, Value *value);

void value_destroy(Value *value);

void value_print(Value *value, bool listAsPairs);

#endif