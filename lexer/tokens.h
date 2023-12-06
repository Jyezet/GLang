#ifndef TOKENS_H
#define TOKENS_H

#pragma once
#include <stdlib.h>
#include "utils.h"
#include "../include/tokenlist.h"

typedef struct Tokenizer{
    char* start;
    char* end;
    char* endOfInput;
} Tokenizer;

typedef struct Token{
    struct Token* prev;
    struct Token* next;
    int type;
    char* content;
} Token;

typedef struct TokenHead{
    Token* first;
    Token* last;
} TokenHead;

void createList(TokenHead* head, int _Type, char* _Content){
    Token* newToken = (Token*) malloc(sizeof(Token));
    newToken->type = _Type;
    newToken->content = duplicate(_Content);

    head->first = newToken;
    head->last = newToken;
}

void addToken(TokenHead* head, int _Type, char* _Content){
    Token* newToken = (Token*) malloc(sizeof(Token));
    newToken->type = _Type;
    newToken->content = duplicate(_Content);

    Token* lastToken = head->last;

    lastToken->next = newToken;
    newToken->prev = lastToken;

    head->last = newToken;
}

#endif