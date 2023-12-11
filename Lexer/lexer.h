#ifndef LEXER_H
#define LEXER_H

#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "../include/utils.h"
#include "../include/definitions.h"
#include "../include/exceptions.h"
#include "tokens.h"

char* loadFile(const char* _Loc){
    FILE* file = fopen(_Loc, "r");

    if(file == NULL){
        THROW_EXC_INFO("File not found", _Loc);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*) malloc(length + 1);
    fread(content, length, 1, file);
    fclose(file);

    content[length] = '\0';
    return content;
}

void skipComments(Tokenizer* _Tok){
    if(_Tok->start[0] == '/' && _Tok->start[1] == '/'){
        while(*_Tok->start != '\r' && *_Tok->start != '\n'){
            _Tok->start++;
        }
    } else if(_Tok->start[0] == '/' && _Tok->start[1] == '*'){
        _Tok->start += 2;

        do {
            _Tok->start = getNextChar(_Tok->start, length(_Tok->start), '*');
        } while(_Tok->start[1] != '/');

        _Tok->start += 2;
    }
}

Token* identifyToken(Tokenizer* _Tok){
    int type;
    char* content;
    static Token lastTok;

    if(_Tok->start >= _Tok->endOfInput){
        Token* blank = (Token*) malloc(sizeof(Token));
        blank->type = BLANK;
        blank->content = const_cast_str("");
        return blank;
    }

    switch(*_Tok->start){
        case DELIM:
            type = END;
            content = const_cast_str(";");
            break;
        case STRING_SYMBOL:
            _Tok->end = _Tok->start;
            do {
                // If tokenizer's end reaches the end of the code, this means the string hasn't been enclosed correctly.
                if(isEndOfString(*_Tok->end)){
                    THROW_EXC("Malformed string literal (Hint: Did you forget to enclose it with 2 double quotes?).");
                }

                _Tok->end = getNextChar(_Tok->end, length(_Tok->start), STRING_SYMBOL);
            } while(_Tok->end[-1] == ESCAPE_SYMBOL);

            type = STRING_LITERAL;
            content = getStrRange(_Tok->start, _Tok->end);
            break;
        case CHAR_SYMBOL:
            _Tok->end = _Tok->start + 2; // Set the tokenizer's end pointer at wherever the closing quote should be, to check it's there
            if(*_Tok->end != CHAR_SYMBOL){
                THROW_EXC("Malformed char literal (Hint: Is your literal more than 1 charater long? Did you miss a quote? Did you escape a single quote? (It's not necessary))");
            }
            type = CHAR_LITERAL;
            content = getStrRange(_Tok->start, _Tok->end);
            break;
        case ' ':
        case '\t':
        case '\f':
        case '\n':
        case '\r':
            type = BLANK;
            content = const_cast_str(" ");
            break;
        case '.':
            if(_Tok->start[1] == '.' && _Tok->start[2] == '.'){
                content = const_cast_str("...");
                type = UNPACK_OP;
                break;
            }

            content = const_cast_str(".");
            type = PROPERTY_OP;
            break;
        case '=':
            if(_Tok->start[1] == '='){ // ==
                content = const_cast_str("==");
                type = EQUALS_OP;
                break;
            }

            if(_Tok->start[1] == '>'){
                content = const_cast_str("=>");
                type = CODE_BLOCK_OP;
                break;
            } 

            content = const_cast_str("=");
            type = ASSIGN_OP; // =
            break;
        case '!':
            if(_Tok->start[1] == '='){ // !=
                content = const_cast_str("!=");
                type = NOT_EQUAL_OP;
                break;
            }
            
            content = const_cast_str("!");
            type = NOT_OP; // !
            break;
        case '?':
            content = const_cast_str("?");
            if(lastTok.type == RET_TYPE_OP){
                type = UNDEF_RETTYPE_OP;
            } else if(lastTok.type == EXTENDS_KEYWORD){
                type = TRAITLESS_EXTENSION_OP;
            } else {
                type = AUTO_CONV_OP;
            }
            break;
        case '>':
            if(_Tok->start[1] == '='){ // >=
                content = const_cast_str(">=");
                type = GREATER_OR_EQUAL_TO_OP;
                break;
            }
            
            content = const_cast_str(">");
            type = GREATER_THAN_OP; // >
            break;
        case '<':
            if(_Tok->start[1] == '='){ // <=
                content = const_cast_str("<=");
                type = LOWER_OR_EQUAL_TO_OP;
                break;
            }
            
            content = const_cast_str("<");
            type = LOWER_THAN_OP; // <
            break;
        case '+':
            if(_Tok->start[1] == '+'){ // ++
                content = const_cast_str("++");
                type = INCREMENT_OP;
                break;
            }
            
            if(_Tok->start[1] == '='){ // +=
                content = const_cast_str("+=");
                type = SUM_ASSIGN_OP;
                break;
            }

            content = const_cast_str("+");
            type = SUM_OP;
            break;
        case '-':
            if(_Tok->start[1] == '-'){
                content = const_cast_str("--");
                type = DECREMENT_OP;
                break;
            }
            
            if(_Tok->start[1] == '='){
                content = const_cast_str("-=");
                type = SUBSTR_ASSIGN_OP;
                break;
            } 
            
            if(_Tok->start[1] == '>'){
                content = const_cast_str("->");
                type = RET_TYPE_OP;
                break;
            }
            
            if(isNumeric(_Tok->start[1])){
                if(isNumeric(*lastTok.content)){
                    type = SUBSTRACT_OP;
                    content = const_cast_str("-");
                    break;
                }

                type = NNUMERIC_LITERAL;
                _Tok->end = _Tok->start;
                int dotsCounter = 0;
                while(isNumeric(_Tok->end[1])){
                    _Tok->end++;
                    if(dotsCounter == 1){ type = FNNUMERIC_LITERAL; }
                    else if(dotsCounter >= 2){ THROW_EXC("Malformed float literal (Hint: Did you accidentally write more than one dot?)"); }

                    if(*_Tok->end == '.'){
                        dotsCounter++;
                    }
                }

                content = getStrRange(_Tok->start, _Tok->end);
                break;
            } 
            
            content = const_cast_str("-");
            type = SUBSTRACT_OP;
            break;
        case '*':
            if(_Tok->start[1] == '*'){
                if(_Tok->start[2] == '='){
                    content = const_cast_str("**=");
                    type = POWER_ASSIGN_OP;

                }

                content = const_cast_str("**");
                type = POWER_OP;
                break;
            }
            
            if(_Tok->start[1] == '='){ // *=
                content = const_cast_str("*=");
                type = MULT_ASSIGN_OP;
                break;
            }

            // In case a multi-line comment is not processed correctly
            if(_Tok->start[1] == '/'){
                type = BLANK;
                content = const_cast_str("  ");
                break;
            }
            
            content = const_cast_str("*");
            type = MULTIPLY_OP; // *
            break;
        case '%':
            content = const_cast_str("%");
            type = MOD_OP;
            break;
        case '/':
            if(_Tok->start[1] == '/' || _Tok->start[1] == '*'){
                skipComments(_Tok);
                type = BLANK;
                content = const_cast_str(""); // Content length is 0, tokenizer's start won't be scrolled wrongly
                break;
            }

            if(_Tok->start[1] == '='){
                content = const_cast_str("/=");
                type = DIVIDE_ASSIGN_OP;
                break;
            }

            content = const_cast_str("/");
            type = DIVIDE_OP;
            break;
        case '(':
            content = const_cast_str("(");
            type = LEFT_PARENTH;
            break;
        case ')':
            content = const_cast_str(")");
            type = RIGHT_PARENTH;
            break;
        case '[':
            content = const_cast_str("[");
            type = LEFT_BRACKET;
            break;
        case ']':
            content = const_cast_str("]");
            type = RIGHT_BRACKET;
            break;
        case '{':
            content = const_cast_str("{");
            type = LEFT_CURLY;
            break;
        case '}':
            content = const_cast_str("}");
            type = RIGHT_CURLY;
            break;
        case '@':
            content = const_cast_str("@");
            type = ATTRIBUTE_OP;
            break;
        case ':':
            if(_Tok->start[1] == ':'){
                content = const_cast_str("::");
                type = SCOPE_RESOLVER_OP;
                break;
            }

            content = const_cast_str(":");
            type = LIBRARY_RESOLVER_OP;
            break;
        default:
            _Tok->end = _Tok->start;

            // Scroll to the next character only if it's a character usable in an identifier ([a-zA-Z0-9]|_)
            while(isValidCharacter(_Tok->end[1])){
                _Tok->end++;
            }

            content = getStrRange(_Tok->start, _Tok->end); // Starting from this line, the tokenizer's pointers won't be used anymore
            
            // Identifiers that start with a number will be processed as numbers
            if(isNumeric(content[0])){
                type = NUMERIC_LITERAL;
                int dotsCounter = 0;
                for(int i = 0; content[i] != '\0'; i++){
                    if(content[i] == '.'){
                        type = FNUMERIC_LITERAL;
                        dotsCounter++;
                    }

                    if(dotsCounter > 1){
                        THROW_EXC("Malformed float literal (Hint: Did you accidentally write more than one dot?)");
                    }
                }
                break;
            }

            if(compare(content, "while")){
                type = WHILE_STMT;
                break;
            }

            if(compare(content, "for")){
                type = FOR_STMT;
                break;
            }

            if(compare(content, "fn")){
                type = FN_KEYWORD;
                break;
            }

            if(compare(content, "in")){
                type = IN_OP;
                break;
            }

            if(compare(content, "if")){
                type = IF_STMT;
                break;
            }

            if(compare(content, "else")){
                type = ELSE_STMT;
                break;
            }

            if(compare(content, "ret")){
                type = RET_KEYWORD;
                break;
            }

            if(compare(content, "let")){
                type = LET_KEYWORD;
                break;
            }

            if(compare(content, "const")){
                type = CONST_KEYWORD;
                break;
            }

            if(compare(content, "mem")){
                type = MEM_KEYWORD;
                break;
            }

            if(compare(content, "bring")){
                type = BRING_KEYWORD;
                break;
            }

            if(compare(content, "include")){
                type = INCLUDE_KEYWORD;
                break;
            }

            if(compare(content, "true") || compare(content, "false")){
                type = BOOL_LITERAL;
                break;
            }

            if(compare(content, "struct")){
                type = STRUCT_KEYWORD;
                break;
            }

            if(compare(content, "trait")){
                type = TRAIT_KEYWORD;
                break;
            }

            if(compare(content, "extends")){
                type = EXTENDS_KEYWORD;
                break;
            }

            type = IDENTIFIER;
            break;
    }

    Token* tok = (Token*) malloc(sizeof(Token));
    tok->type = type;
    tok->content = duplicate(content);

    // Don't cache blank tokens
    if(tok->type != BLANK){
        lastTok = *tok;
    }

    return tok;
}

TokenHead* parseTokens(Tokenizer* _Tok){
    TokenHead* head = (TokenHead*) malloc(sizeof(TokenHead));
    Token* newToken;
    int first = 1;

    while(_Tok->start < _Tok->endOfInput){
        newToken = identifyToken(_Tok);
        _Tok->start += length(newToken->content);

        if(newToken->type != BLANK){
            if(first){
                createList(head, newToken->type, newToken->content);
                first = 0;
            } else {
                addToken(head, newToken->type, newToken->content);
            }
        }

        free(newToken);
    }

    return head;
}

TokenHead* lexer_entryPoint(const char* filename){
    char* code = loadFile(filename);
    Tokenizer* tokenizer = (Tokenizer*) malloc(sizeof(Tokenizer));
    tokenizer->start = code;
    tokenizer->endOfInput = code + length(code);
    TokenHead* tokens = parseTokens(tokenizer);

    free(tokenizer);
    return tokens;
}

#endif