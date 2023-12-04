#ifndef LEXER_H
#define LEXER_H

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "../include/definitions.h"
#include "../include/exceptions.h"
#include "tokens.h"
#include "tokenizer.h"

char* loadFile(const char* _Loc){
    FILE* file = fopen(_Loc, "r");

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
        while(!isNewline(*_Tok->start)){
            _Tok->start ++;
        }
        _Tok->start ++;
        return;
    }
    
    if(_Tok->start[0] == '/' && _Tok->start[1] == '*'){
        // Jump to the next position until a */ is reached
        while(_Tok->start[0] != '*' && _Tok->start[1] != '/'){
            if(isEndOfString(*_Tok->start)){
                // Comment hasn't been enclosed correctly, throw error
                THROW_EXC("Unenclosed multi-line comment");
            }

            _Tok->start ++;
        }

        _Tok->start += 2;
    }
}

void skipSpaces(Tokenizer* _Tok){
    while(isEmpty(*_Tok->start)){
        _Tok->start++;
    }
}

double parseNumber(const char* _Text, int base){
    double number = 0;
    bool negative = false;
    bool decimal = false;
    double decimal_pos = 1;
    int constant = 10;
    for(int i = 0; !isEndOfString(_Text[i]); i++){
        if(isDigit(_Text[i])){
            number = number * base + _Text[i] - 48;
            if(decimal) { decimal_pos *= base; }
        } else if(_Text[i] == '-'){
            negative = true;
        } else if(_Text[i] == '.'){
            decimal = true;
        } else {
            printf("Error, '%c' is not numeric", _Text[i]);
        }
    }
    
    if(negative) { number *= -1; }
    number /= decimal_pos;
    return number;
}

Token* identifyToken(Tokenizer* _Tok){
    int type;
    char* content;

    // TODO: Fix bug where first token is processed twice
    switch(*_Tok->start){
        case DELIM:
            type = END;
            content = ";";
            _Tok->start += strlen(content);
            break;
        case STRING_SYMBOL:
            do {
                // If tokenizer's end reaches the end of the code, this means the string hasn't been enclosed correctly.
                if(isEndOfString(*_Tok->end)){
                    THROW_EXC("Malformed string literal (Hint: Did you forget to enclose it with 2 double quotes?).");
                }

                _Tok->end++;
            } while(*_Tok->end != STRING_SYMBOL && _Tok->end[-1] != ESCAPE_SYMBOL); // do while because the first symbol is already a quote

            type = STRING_LITERAL;
            content = getStrRange(_Tok->start, _Tok->end);
            _Tok->start = _Tok->end + 1;
            break;
        case CHAR_SYMBOL:
            _Tok->end = _Tok->start + 2; // Set the tokenizer's end pointer at wherever the closing quote should be, to check it's there
            if(*_Tok->end != CHAR_SYMBOL){
                THROW_EXC("Malformed char literal (Hint: Is your literal more than 1 charater long? Did you miss a quote?)");
            }
            type = CHAR_LITERAL;
            content = (char*) malloc(1);
            *content = _Tok->start[1];
            _Tok->start = _Tok->end + 1;
            break;
        case ' ':
        case '\t':
        case '\f':
        case '\n':
        case '\r':
            skipSpaces(_Tok);
            break;
        case '=':
            if(_Tok->start[1] == '='){ // ==
                content = "==";
                type = EQUALS_OP;
                _Tok->start += strlen(content);
                break;
            }

            content = "=";
            type = ASSIGN_OP; // =
            _Tok->start += strlen(content);
            break;
        case '!':
            if(_Tok->start[1] == '='){ // !=
                content = "!=";
                type = NOT_EQUAL_OP;
                _Tok->start += strlen(content);
                break;
            }
            
            content = "!";
            type = NOT_OP; // !
            _Tok->start += strlen(content);
            break;
        case '>':
            if(_Tok->start[1] == '='){ // >=
                content = ">=";
                type = GREATER_OR_EQUAL_TO_OP;
                _Tok->start += strlen(content);
                break;
            }
            
            content = ">";
            type = GREATER_THAN_OP; // >
            _Tok->start += strlen(content);
            break;
        case '<':
            if(_Tok->start[1] == '='){ // <=
                content = "<=";
                type = LOWER_OR_EQUAL_TO_OP;
                _Tok->start += strlen(content);
                break;
            }
            
            content = "<";
            type = LOWER_THAN_OP; // <
            _Tok->start += strlen(content);
            break;
        case '+':
            if(_Tok->start[1] == '+'){ // ++
                content = "+";
                type = INCREMENT_OP;
                _Tok->start += strlen(content);
                break;
            }
            
            if(_Tok->start[1] == '='){ // +=
                content = "=";
                type = SUM_ASSIGN_OP;
                _Tok->start += strlen(content);
                break;
            }

            content = "+";
            type = SUM_OP; // +
            _Tok->start += strlen(content);
            break;
        case '-':
            if(_Tok->start[1] == '-'){ // --
                content = "--";
                type = DECREMENT_OP;
                _Tok->start += strlen(content);
                break;
            }
            
            if(_Tok->start[1] == '='){ // -=
                content = "-=";
                type = SUBSTR_ASSIGN_OP;
                _Tok->start += strlen(content);
                break;
            } 
            
            if(_Tok->start[1] == '>'){ // ->
                content = "->";
                type = CODE_BLOCK_OP;
                _Tok->start += strlen(content);
                break;
            } 
            
            // TODO: Find a way to distinguish between a negative number and a substraction without spaces (eg: 4 -2)
            // Suggestion: Check if there's an arithmetical operator before this token, if there isn't, turn it into a positive number
            // And add a substraction operator
            if(isNumeric(_Tok->start[1])){ // Negative number
                type = NUMERIC_LITERAL;
                _Tok->end = _Tok->start;
                while(isNumeric(*_Tok->end)){
                    _Tok->end++;
                }

                content = getStrRange(_Tok->start, _Tok->end);
                _Tok->start = _Tok->end + 1;
                break;
            } 
            
            content = "-";
            type = SUBSTRACT_OP; // -
            _Tok->start += strlen(content);
            break;
        case '*':
            if(_Tok->start[1] == '*'){ // **
                content = "**";
                type = POWER_OP;
                _Tok->start += strlen(content);
                break;
            }
            
            if(_Tok->start[1] == '='){ // *=
                content = "*=";
                type = MULT_ASSIGN_OP;
                _Tok->start += strlen(content);
                break;
            }
            
            content = "*";
            type = MULTIPLY_OP; // *
            _Tok->start += strlen(content);
            break;
        case '%':
            content = "%";
            type = MOD_OP; // %
            _Tok->start += strlen(content);
            break;
        case '/':
            if(_Tok->start[1] == '/' || _Tok->start[1] == '*'){ // /* or // (Comment)
                skipComments(_Tok);
                break;
            }

            if(_Tok->start[1] == '='){ // /=
                content = "/=";
                type = DIVIDE_ASSIGN_OP;
                _Tok->start += strlen(content);
                break;
            }

            content = "/";
            type = DIVIDE_OP; // /
            _Tok->start += strlen(content);
            break;
        case '(':
            content = "(";
            type = LEFT_PARENTH;
            _Tok->start += strlen(content);
            break;
        case ')':
            content = ")";
            type = RIGHT_PARENTH;
            _Tok->start += strlen(content);
            break;
        case '[':
            content = "[";
            type = LEFT_BRACKET;
            _Tok->start += strlen(content);
            break;
        case ']':
            content = "]";
            type = RIGHT_BRACKET;
            _Tok->start += strlen(content);
            break;
        case '{':
            content = "{";
            type = LEFT_CURLY;
            _Tok->start += strlen(content);
            break;
        case '}':
            content = "}";
            type = RIGHT_CURLY;
            _Tok->start += strlen(content);
            break;
        case '@':
            content = "@";
            type = ATTRIBUTE_OP;
            _Tok->start += strlen(content);
            break;
        case ':':
            if(_Tok->start[1] == ':'){
                content = "::";
                type = SCOPE_RESOLVER_OP;
                _Tok->start += strlen(content);
                break;
            }

            content = ":";
            type = LIBRARY_RESOLVER_OP;
            _Tok->start += strlen(content);
            break;
        default:
            _Tok->end = _Tok->start;

            // Scroll to the next character only if it's a character usable in an identifier ([a-zA-Z0-9]|_)
            while(isIdentifier(_Tok->end[1])){
                _Tok->end++;
            }

            content = getStrRange(_Tok->start, _Tok->end); // Starting from this line, the tokenizer's pointers won't be used anymore
            _Tok->start += strlen(content);
            
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

            if(strcmp(content, "while") == 0){
                type = WHILE_STMT;
                break;
            }

            if(strcmp(content, "for") == 0){
                type = FOR_STMT;
                break;
            }

            if(strcmp(content, "fn") == 0){
                type = FN_KEYWORD;
                break;
            }

            if(strcmp(content, "if") == 0){
                type = IF_STMT;
                break;
            }

            if(strcmp(content, "else") == 0){
                type = ELSE_STMT;
                break;
            }

            if(strcmp(content, "ret") == 0){
                type = RET_KEYWORD;
                break;
            }

            if(strcmp(content, "let") == 0){
                type = LET_KEYWORD;
                break;
            }

            if(strcmp(content, "const") == 0){
                type = CONST_KEYWORD;
                break;
            }

            if(strcmp(content, "mem") == 0){
                type = MEM_KEYWORD;
                break;
            }

            if(strcmp(content, "bring") == 0){
                type = BRING_KEYWORD;
                break;
            }

            if(strcmp(content, "include") == 0){
                type = INCLUDE_KEYWORD;
                break;
            }

            if(strcmp(content, "true") == 0 || strcmp(content, "false") == 0){
                type = BOOL_LITERAL;
                break;
            }

            type = IDENTIFIER;
            break;
    }

    Token* tok = (Token*) malloc(sizeof(Token));
    tok->type = type;
    tok->content = strdup(content);
    return tok;
}

TokenHead* parseTokens(Tokenizer* _Tok){
    TokenHead* head = (TokenHead*) malloc(sizeof(TokenHead));
    Token* newToken;
    int first = 1;

    while(!isEndOfString(*_Tok->start)){
        newToken = identifyToken(_Tok);

        if(first == 1)
        { createList(head, newToken->type, newToken->content); first = 0; continue; }

        addToken(head, newToken->type, newToken->content);
        free(newToken);
        _Tok->start++;
    }

    return head;
}

TokenHead* lexer(char* filename){
    //char* code = loadFile(filename);
    char code[] = "let var = \"code\"; let var2 = \"code2\";\nprint(\"code {} code {}\", [$var, $var2]);";
    Tokenizer tokenizer;
    tokenizer.start = code;
    TokenHead* tokens = parseTokens(&tokenizer);
    return tokens;
}

#endif