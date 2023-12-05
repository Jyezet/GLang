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
        while(*_Tok->start != '\r' && *_Tok->start != '\n'){
            _Tok->start++;
        }
    } else if(_Tok->start[0] == '/' && _Tok->start[1] == '*'){
        // Jump to the next position until a */ is at the left of the tokenizer's left pointer
        while(_Tok->start[-1] != '/' && _Tok->start[-2] != '*'){
            if(isEndOfString(*_Tok->start)){
                // Comment hasn't been enclosed correctly, throw error
                THROW_EXC("Unenclosed multi-line comment (Hint: Did you forget to write an '*/' encloser?).");
            }

            _Tok->start++;
        }
    }
}

double parseNumber(const char* _Text, int base){
    // TODO: Scientific notation
    double number = 0;
    bool negative = false;
    bool decimal = false;
    int decimal_pos = 1;
    int constant = 10;
    for(int i = 0; !isEndOfString(_Text[i]); i++){
        if(isDigit(_Text[i])){
            number = number * base + _Text[i] - '0';
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
    static Token lastTok;

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
            type = BLANK;
            content = "";
            _Tok->start++; // Skip blank characters
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
            
            if(isNumeric(_Tok->start[1])){
                if(isNumeric(*lastTok.content)){
                    type = SUBSTRACT_OP;
                    content = "-";
                    _Tok->start += strlen(content);
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

            // In case a multi-line comment is not processed correctly
            if(_Tok->start[1] == '/'){
                _Tok->start += 2;
                type = BLANK;
                content = "";
                break;
            }
            
            content = "*";
            type = MULTIPLY_OP; // *
            _Tok->start += strlen(content);
            break;
        case '%':
            content = "%";
            type = MOD_OP;
            _Tok->start += strlen(content);
            break;
        case '/':
            if(_Tok->start[1] == '/' || _Tok->start[1] == '*'){ // /* or // (Comment)
                skipComments(_Tok);
                type = BLANK;
                content = "";
                break;
            }

            if(_Tok->start[1] == '='){
                content = "/=";
                type = DIVIDE_ASSIGN_OP;
                _Tok->start += strlen(content);
                break;
            }

            content = "/";
            type = DIVIDE_OP;
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
            while(isValidCharacter(_Tok->end[1])){
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

    // Don't cache blank tokens!
    if(tok->type != BLANK){
        lastTok = *tok;
    }

    return tok;
}

TokenHead* parseTokens(Tokenizer* _Tok, char* endOfCode){
    TokenHead* head = (TokenHead*) malloc(sizeof(TokenHead));
    Token* newToken;
    int first = 1;

    while(_Tok->start < endOfCode){
        newToken = identifyToken(_Tok);

        if(first == 1 && newToken && newToken->type != BLANK)
        { createList(head, newToken->type, newToken->content); first = 0; continue; }

        if(newToken->type != BLANK){
            addToken(head, newToken->type, newToken->content);
        }

        free(newToken);
        //_Tok->start++; I'm actually leaving this bug causing line as a monument to human stupidity (Tokenizer scrolling is already done inside the identifyToken function, causing double scrolling and thus skipping some chars)
    }

    return head;
}

TokenHead* lexer(char* filename){
    //char* code = loadFile(filename);
    char code[] = "let var = \"code\"; let var2 = \"code2\";\nprint(\"code {} code {}\", [$var, $var2]);";
    Tokenizer tokenizer;
    tokenizer.start = code;
    TokenHead* tokens = parseTokens(&tokenizer, code + strlen(code));
    return tokens;
}

#endif