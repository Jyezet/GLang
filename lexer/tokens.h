#ifndef TOKENS_H
#define TOKENS_H

#pragma once
#include <stdlib.h>
#include "../include/tokenlist.h"

typedef struct Token{
    struct Token* prev;
    struct Token* next;
    int type;
    char* content;
} Token;

Token* getLastToken(Token* _Token){
    Token* currToken = _Token;
    while(currToken->next != NULL){
        currToken = currToken->next;
    }

    return currToken;
}

void addToken(Token* head, int _Type, char* _Content){
    Token* newToken = (Token*) malloc(sizeof(Token));
    newToken->type = _Type;
    newToken->content = _Content;

    // Avoid overwriting an existing token, instead, add the new one at the end of the list
    Token* lastToken = getLastToken(head);
    lastToken->next = newToken;
    newToken->prev = lastToken;
}

#endif