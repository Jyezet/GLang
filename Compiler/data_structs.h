#ifndef DATA_STRUCTS_INTERP_H
#define DATA_STRUCTS_INTERP_H

#pragma once
#include <stdlib.h>

typedef struct{
    void** data;
    int size;
} Stack;

void* stackGet(Stack* stack){
    return stack->data[stack->size - 1];
}

void stackRem(Stack* stack){
    free(stack->data[stack->size - 1]);
    stack->data[--stack->size] = NULL;
    stack->data = realloc(stack->data, stack->size * sizeof(void*));
}

void stackAdd(Stack* stack, void* data){
    stack->data = realloc(stack->data, ++stack->size * sizeof(void*));
    stack->data[stack->size - 1] = data;
}

#endif