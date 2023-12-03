#pragma once
#include <stdio.h>

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define STRING_SYMBOL '"'
#define CHAR_SYMBOL '\''
#define ESCAPE_SYMBOL '\\'
#define DELIM ';'
#define STRINGIFY(expr) #expr
#define ASSERT(expr, msg) if(!(expr)){printf("Assertion failed: %s, at line: %i, file: %s; %s\n", STRINGIFY(expr), __LINE__, __FILE__, msg);}

#endif