#ifndef PARSER_H
#define PARSER_H

#pragma once
#include "../include/exceptions.h"
#include "../include/tokenlist.h"
#include "../include/utils.h"
#include "../lexer/tokens.h"
#include "syntax_tree.h"

void parseToken(ParsedTree* tree, Token* token){
    /* --- COUNTERS --- */
    static int OPEN_PARENTH_BRACES_COUNTER = 0; // Increment when hitting a (, decrement upon hitting a )
    static int OPEN_CURLY_BRACES_COUNTER = 0; // Increment when hitting a {, decrement upon hitting a }
    static int OPEN_BRACKET_BRACES_COUNTER = 0; // Increment when hitting a [, decrement upon hitting a ]
    static int OPEN_CURLY_BRACES_CODEBLOCK = -1;

    /* --- GENERAL FLAGS --- */
    static int IS_CODE_BLOCK; // Set to true if => operator is read (And record OPEN_CURLY_BRACES_COUNTER), set to false once OPEN_CURLY_BRACES_COUNTER goes back to its original value
    static int IS_GLOBAL_SCOPE;
    if(!IS_CODE_BLOCK && OPEN_CURLY_BRACES_COUNTER == 0){
        IS_GLOBAL_SCOPE = 1;
    } else {
        IS_GLOBAL_SCOPE = 0;
    }

    /* --- STATEMENT SPECIFIC FLAGS --- */
    static int DECL_SYMBOL_FLAG; // Set to true if the line currently being parsed declares a new symbol
    static int DECL_CONSTANT_FLAG;
    static int DECL_VAR_FLAG;
    static int DECL_FN_FLAG;

    if(token->type == LET_KEYWORD){
        DECL_SYMBOL_FLAG = 1;
        DECL_VAR_FLAG = 1;
    }

    else if(token->type == CONST_KEYWORD){
        DECL_SYMBOL_FLAG = 1;
        DECL_CONSTANT_FLAG = 1;
    }

    else if(token->type == FN_KEYWORD){
        DECL_SYMBOL_FLAG = 1;
        DECL_FN_FLAG = 1;
    }

    if(token->type == LEFT_BRACKET){
        OPEN_BRACKET_BRACES_COUNTER++;
    }

    else if(token->type == RIGHT_BRACKET){
        OPEN_BRACKET_BRACES_COUNTER--;
    }

    else if(token->type == LEFT_CURLY){
        OPEN_CURLY_BRACES_COUNTER++;
    }

    else if(token->type == RIGHT_CURLY){
        OPEN_CURLY_BRACES_COUNTER--;

        if(OPEN_CURLY_BRACES_COUNTER == OPEN_CURLY_BRACES_CODEBLOCK){
            OPEN_CURLY_BRACES_CODEBLOCK = -1;
            IS_CODE_BLOCK = 0;
            DECL_FN_FLAG = 0;
        }
    }

    else if(token->type == LEFT_PARENTH){
        OPEN_PARENTH_BRACES_COUNTER++;
    }

    else if(token->type == RIGHT_PARENTH){
        OPEN_PARENTH_BRACES_COUNTER--;
    }

    else if(token->type == IDENTIFIER){
        if(DECL_SYMBOL_FLAG){
            tree->symbols++;
        }
    }
    
    // Upon reaching a statement's end, reset all statement specific flags
    else if(token->type == END){
        DECL_SYMBOL_FLAG = 0;
        DECL_CONSTANT_FLAG = 0;
        DECL_VAR_FLAG = 0;
        DECL_FN_FLAG = 0;
    }

    else if(token->type == CODE_BLOCK_OP){
        IS_CODE_BLOCK = 1;
        OPEN_CURLY_BRACES_CODEBLOCK = OPEN_CURLY_BRACES_COUNTER;
    }
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