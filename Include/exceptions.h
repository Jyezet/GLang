#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#pragma once
#include <stdio.h>
#include <stdlib.h>

#define THROW_EXC(type) printf("Error in line %i, file %s: %s\n", __LINE__, __FILE__, type); exit(-1);
#define THROW_EXC_INFO(type, additional_info) printf("Error in line %i, file %s; %s: %s.\n", __LINE__, __FILE__, type, additional_info); exit(-1);

#endif