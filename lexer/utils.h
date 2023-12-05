#ifndef UTILS_H
#define UTILS_H

#pragma once
#include <stdlib.h>
#include <stdbool.h>

extern inline bool isLetter(char _Character){
    return (_Character >= 'a' && _Character <= 'z') || (_Character >= 'A' && _Character <= 'Z');
}

extern inline bool isDigit(char _Character){
    return _Character >= '0' && _Character <= '9';
}

extern inline bool isNumeric(char _Character){
    return isDigit(_Character) || _Character == '.' || _Character == '-';
}

extern inline bool isEndOfString(char _Character){
    return _Character == '\0' || _Character == -1; // EOF == -1
}

extern inline bool isIdentifier(char _Character){
    return isDigit(_Character) || isLetter(_Character) || _Character == '_';
}

extern inline bool isValidCharacter(char _Character){
    return isNumeric(_Character) || isLetter(_Character) || _Character == '_';
}

extern inline bool isSymbol(char _Character){
    return !isLetter(_Character) && !isNumeric(_Character);
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