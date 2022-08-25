#ifndef PRINT_H
#define PRINT_H

#include "value.h"

void value_print(Value *value);
void value_print_debug_offset(Value *value, int offset);
void value_print_debug(Value *value);

#endif