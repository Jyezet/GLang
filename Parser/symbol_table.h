#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#pragma once
#include "../include/utils.h"
#include "../include/exceptions.h"

typedef struct{
    char* name;
    char* datatype;
    char* scope;
    int size;
    int isFunction;
} SymbolTable_row;

typedef struct{
    int size;
    int* freeSpots;
    SymbolTable_row** data;
} SymbolTable;

const int nextFreeSpot(SymbolTable* table){
    for(int i = 0; i < table->size; i++){
        if(table->freeSpots[i]){
            return i;
        }
    }

    return -1;
}

void del_row(SymbolTable* table, int row){
    if(row > table->size){
        return;
    }

    row--;
    free(table->data[row]);
    table->freeSpots[row] = 1;
}

void add_row(SymbolTable* table, const char* name, const char* datatype, const char* scope, int size, int isFunction){
    SymbolTable_row* row = (SymbolTable_row*) malloc(sizeof(SymbolTable_row));
    row->name = duplicate(name);
    row->datatype = duplicate(datatype);
    row->scope = duplicate(scope);
    row->size = duplicate(size);
    row->isFunction = isFunction;

    const int freeSpot = nextFreeSpot(table);

    if(freeSpot == -1){
        THROW_EXC("Symbol table full");
    }

    table->data[freeSpot] = row;
    table->freeSpots[freeSpot] = 0;
}

SymbolTable* new_table(int size){
    SymbolTable* table = (SymbolTable*) malloc(sizeof(SymbolTable));
    table->size = size;
    table->data = (SymbolTable_row**) malloc(table->size * sizeof(SymbolTable_row*));
    table->freeSpots = (int*) malloc(table->size * sizeof(int));
    set_memory(table->freeSpots, 1, size);
    return table;
}

#endif