#ifndef PARSER_H
#define PARSER_H

#pragma once
#include "../include/exceptions.h"
#include "../include/utils.h"
#include "../lexer/tokens.h"
#include "syntax_tree.h"

void parseToken(ParsedTree* tree, Token* token){

}

ParsedTree* parser_entryPoint(TokenHead* token_head){
    Token* curr = token_head->first;
    ParsedTree* tree;

    while(curr->next != NULL){
        curr = curr->next;

        if(!curr->ignore){
            parseToken(tree, curr);
        }
    }

    return tree;
}

#endif