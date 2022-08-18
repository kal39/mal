#ifndef PARSER_H
#define PARSER_H

#include "common.h"
#include "reader.h"
#include "value.h"

Value *read(Reader *reader, ErrorTracker *errorTracker);

#endif