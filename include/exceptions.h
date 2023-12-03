#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#pragma once
#include <stdio.h>
#include <stdlib.h>

#define THROW_EXC(type) printf("Error in line %i file %s: %s", __LINE__, __FILE__, type); exit(-1);

#endif