#ifndef LEXER_H
#define LEXER_H

#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "../include/definitions.h"
#include "../include/exceptions.h"
#include "tokens.h"

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
    bool decimal = false;
    int decimal_pos = 1;
    int constant = 10;
    for(int i = 0; !isEndOfString(_Text[i]); i++){
        if(isDigit(_Text[i])){
            number = number * base + _Text[i] - '0';
            if(decimal) { decimal_pos *= base; }
        } else if(_Text[i] == '-' && i != 0){
            printf("Error, '-' symbol can only be at the beginning of a number.");
        } else if(_Text[i] == '.'){
            decimal = true;
        } else {
            printf("Error, '%c' is not numeric", _Text[i]);
            exit(-1);
        }
    }
    
    if(_Text[0] == '-') { number *= -1; }
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
            content = duplicate(";");
            break;
        case STRING_SYMBOL:
            _Tok->end = _Tok->start;
            do {
                // If tokenizer's end reaches the end of the code, this means the string hasn't been enclosed correctly.
                if(isEndOfString(*_Tok->end)){
                    THROW_EXC("Malformed string literal (Hint: Did you forget to enclose it with 2 double quotes?).");
                }

                _Tok->end = getNextChar(_Tok->end, STRING_SYMBOL);
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
            content = duplicate(" ");
            break;
        case '=':
            if(_Tok->start[1] == '='){ // ==
                content = duplicate("==");
                type = EQUALS_OP;
                break;
            }

            content = duplicate("=");
            type = ASSIGN_OP; // =
            break;
        case '!':
            if(_Tok->start[1] == '='){ // !=
                content = duplicate("!=");
                type = NOT_EQUAL_OP;
                break;
            }
            
            content = duplicate("!");
            type = NOT_OP; // !
            break;
        case '>':
            if(_Tok->start[1] == '='){ // >=
                content = duplicate(">=");
                type = GREATER_OR_EQUAL_TO_OP;
                break;
            }
            
            content = duplicate(">");
            type = GREATER_THAN_OP; // >
            break;
        case '<':
            if(_Tok->start[1] == '='){ // <=
                content = duplicate("<=");
                type = LOWER_OR_EQUAL_TO_OP;
                break;
            }
            
            content = duplicate("<");
            type = LOWER_THAN_OP; // <
            break;
        case '+':
            if(_Tok->start[1] == '+'){ // ++
                content = duplicate("++");
                type = INCREMENT_OP;
                break;
            }
            
            if(_Tok->start[1] == '='){ // +=
                content = duplicate("+=");
                type = SUM_ASSIGN_OP;
                break;
            }

            content = duplicate("+");
            type = SUM_OP; // +
            break;
        case '-':
            if(_Tok->start[1] == '-'){ // --
                content = duplicate("--");
                type = DECREMENT_OP;
                break;
            }
            
            if(_Tok->start[1] == '='){ // -=
                content = duplicate("-=");
                type = SUBSTR_ASSIGN_OP;
                break;
            } 
            
            if(_Tok->start[1] == '>'){ // ->
                content = duplicate("->");
                type = CODE_BLOCK_OP;
                break;
            } 
            
            if(isNumeric(_Tok->start[1])){
                if(isNumeric(*lastTok.content)){
                    type = SUBSTRACT_OP;
                    content = duplicate("-");
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
            
            content = duplicate("-");
            type = SUBSTRACT_OP;
            break;
        case '*':
            if(_Tok->start[1] == '*'){
                content = duplicate("**");
                type = POWER_OP;
                break;
            }
            
            if(_Tok->start[1] == '='){ // *=
                content = duplicate("*=");
                type = MULT_ASSIGN_OP;
                break;
            }

            // In case a multi-line comment is not processed correctly
            if(_Tok->start[1] == '/'){
                type = BLANK;
                content = duplicate("  ");
                break;
            }
            
            content = duplicate("*");
            type = MULTIPLY_OP; // *
            break;
        case '%':
            content = duplicate("%");
            type = MOD_OP;
            break;
        case '/':
            if(_Tok->start[1] == '/' || _Tok->start[1] == '*'){
                skipComments(_Tok);
                type = BLANK;
                content = duplicate("");
                break;
            }

            if(_Tok->start[1] == '='){
                content = duplicate("/=");
                type = DIVIDE_ASSIGN_OP;
                break;
            }

            content = duplicate("/");
            type = DIVIDE_OP;
            break;
        case '(':
            content = duplicate("(");
            type = LEFT_PARENTH;
            break;
        case ')':
            content = duplicate(")");
            type = RIGHT_PARENTH;
            break;
        case '[':
            content = duplicate("[");
            type = LEFT_BRACKET;
            break;
        case ']':
            content = duplicate("]");
            type = RIGHT_BRACKET;
            break;
        case '{':
            content = duplicate("{");
            type = LEFT_CURLY;
            break;
        case '}':
            content = duplicate("}");
            type = RIGHT_CURLY;
            break;
        case '@':
            content = duplicate("@");
            type = ATTRIBUTE_OP;
            break;
        case ':':
            if(_Tok->start[1] == ':'){
                content = duplicate("::");
                type = SCOPE_RESOLVER_OP;
                break;
            }

            content = duplicate(":");
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

            type = IDENTIFIER;
            break;
    }

    Token* tok = (Token*) malloc(sizeof(Token));
    tok->type = type;
    tok->content = duplicate(content);

    // Don't cache blank tokens!
    if(tok->type != BLANK){
        lastTok = *tok;
    }

    return tok;
}

TokenHead* parseTokens(Tokenizer* _Tok){
    TokenHead* head = (TokenHead*) malloc(sizeof(TokenHead));
    Token* newToken;
    bool first = true;

    while(_Tok->start < _Tok->endOfInput){
        newToken = identifyToken(_Tok);
        _Tok->start += length(newToken->content);

        if(newToken->type != BLANK){
            if(first){
                createList(head, newToken->type, newToken->content);
                first = false;
            } else {
                addToken(head, newToken->type, newToken->content);
            }
        }

        free(newToken);
    }

    return head;
}

TokenHead* lexer_entryPoint(char* filename){
    char* code = loadFile(filename);
    Tokenizer* tokenizer = (Tokenizer*) malloc(sizeof(Tokenizer));
    tokenizer->start = code;
    tokenizer->endOfInput = code + length(code);
    TokenHead* tokens = parseTokens(tokenizer);
    return tokens;
}

#endif