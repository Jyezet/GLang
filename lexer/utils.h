#ifndef UTILS_H
#define UTILS_H

#pragma once
#include <stdbool.h>
#include <stdlib.h>

extern inline bool isEmpty(char _Character){
    return _Character == ' ' || _Character == '\t' || _Character == '\f';
}

extern inline bool isNewline(char _Character){
    return _Character == '\n' || _Character == '\r';
}

extern inline bool isLetter(char _Character){
    return (_Character >= 'a' && _Character <= 'z') || (_Character >= 'A' && _Character <= 'Z');
}

extern inline bool isNumber(char _Character){
    return (_Character >= '0' && _Character <= '9') || _Character == '.' || _Character == '-';
}

extern inline bool isEndOfString(char _Character){
    return _Character == '\0';
}

extern inline bool isIdentifier(char _Character){
    return (_Character >= '0' && _Character <= '9') || isLetter(_Character) || _Character == '_';
}

extern inline bool isSymbol(char _Character){
    return !isLetter(_Character) && !isNumber(_Character);
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

#endif