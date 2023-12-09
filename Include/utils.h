#ifndef LEXER_UTILS_H
#define LEXER_UTILS_H

#pragma once
#include <stdlib.h>

extern inline int isLetter(char _Character){
    return (_Character >= 'a' && _Character <= 'z') || (_Character >= 'A' && _Character <= 'Z');
}

extern inline int isDigit(char _Character){
    return _Character >= '0' && _Character <= '9';
}

extern inline int isNumeric(char _Character){
    return isDigit(_Character) || _Character == '.' || _Character == '-';
}

extern inline int isEndOfString(char _Character){
    return _Character == '\0' || _Character == -1; // EOF == -1
}

extern inline int isIdentifier(char _Character){
    return isDigit(_Character) || isLetter(_Character) || _Character == '_';
}

extern inline int isValidCharacter(char _Character){
    return isNumeric(_Character) || isLetter(_Character) || _Character == '_';
}

extern inline int isEmpty(char _Character){
    return _Character != ' ' && _Character != '\t' && _Character != '\f' && _Character != '\n' && _Character != '\r';
}

extern inline int isSymbol(char _Character){
    return !isLetter(_Character) && !isNumeric(_Character) && !isEmpty(_Character);
}

char* getStrRange(char* start, char* end){
    int length = end - start + 2; // Sum 1 for the null terminator, 1 for the final character
    char* str = (char*) malloc(length);
    for(int i = 0; start + i != end; i++){
        str[i] = start[i];
    }
    
    str[length - 2] = *end;
    str[length - 1] = '\0';
    return str;
}

char* getNextChar(char* ptr, int len, char target){
    int scrolled = 0;
    do {
        ptr++;
        scrolled++;
    } while(*ptr != target && scrolled <= len);
    return ptr;
}

int length(const char* str){
    int length = 0;
    for(int i = 0; str[i] != '\0'; i++){
        length++;
    }

    return length;
}

int compare(const char* str1, const char* str2){
    if(length(str1) != length(str2)){
        return 0;
    }

    for(int i = 0; str1[i] != '\0' && str2[i] != '\0'; i++){
        if(str1[i] != str2[i]){
            return 0;
        }
    }

    return 1;
}

char* duplicate(const char* str){
    int newStrLength = length(str) + 1;
    char* newStr = (char*) malloc(newStrLength);
    for(int i = 0; str[i] != '\0'; i++){
        newStr[i] = str[i];
    }

    newStr[newStrLength - 1] = '\0';
    return newStr;
}

#define const_cast_str(arg) duplicate(arg)

char* concatenate(const char* str1, const char* str2){
    int str1offset = length(str1);
    int newStrLength = str1offset + length(str2) + 1;
    char* newStr = (char*) malloc(newStrLength);
    for(int i = 0; str1[i] != '\0'; i++){
        newStr[i] = str1[i];
    }

    for(int i = 0; str2[i] != '\0'; i++){
        newStr[i + str1offset] = str2[i];
    }

    newStr[newStrLength - 1] = '\0';
    return newStr;
}

#endif