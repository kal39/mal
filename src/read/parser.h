#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "scanner.h"
#include "value/value.h"

Value *parse(Scanner *scanner);

#endif