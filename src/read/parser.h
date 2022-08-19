#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "reader.h"
#include "value/value.h"

Value *read(Tokenizer *tokenizer);

#endif