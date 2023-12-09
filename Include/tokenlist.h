#ifndef TOKENLIST_H
#define TOKENLIST_H

enum TOKEN{
    EOI,
    END,
    BLANK,
    IDENTIFIER, // Variable/function/enum/class/etc names

    /* --- Literals --- */
    STRING_LITERAL,    
    CHAR_LITERAL,
    NUMERIC_LITERAL, // 4
    FNUMERIC_LITERAL, // 4.2
    NNUMERIC_LITERAL, // -4
    FNNUMERIC_LITERAL, // -4.2
    BOOL_LITERAL,

    /* --- Statements --- */
    IF_STMT,
    ELSE_STMT,
    WHILE_STMT,
    FOR_STMT,

    /* --- Operators --- */
    ASSIGN_OP,
    EQUALS_OP,
    NOT_OP,
    NOT_EQUAL_OP,
    GREATER_THAN_OP,
    LOWER_THAN_OP,
    GREATER_OR_EQUAL_TO_OP,
    LOWER_OR_EQUAL_TO_OP,
    INCREMENT_OP,
    DECREMENT_OP,
    SUM_ASSIGN_OP,
    SUBSTR_ASSIGN_OP,
    SUM_OP,
    SUBSTRACT_OP,
    MULTIPLY_OP,
    MULT_ASSIGN_OP,
    DIVIDE_OP,
    DIVIDE_ASSIGN_OP,
    POWER_OP,
    MOD_OP,
    CODE_BLOCK_OP, // => operator, indicates what follows is a code block
    RET_TYPE_OP, // -> operator, indicates what follows is a function's return datatype
    SCOPE_RESOLVER_OP,
    LIBRARY_RESOLVER_OP,
    ATTRIBUTE_OP, // @ operator, to indicate stuff to the interpreter

    /* --- Braces --- */
    LEFT_PARENTH,
    RIGHT_PARENTH,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_CURLY,
    RIGHT_CURLY,

    RET_KEYWORD,
    LET_KEYWORD,
    CONST_KEYWORD,
    MEM_KEYWORD,
    BRING_KEYWORD,
    FN_KEYWORD,
    INCLUDE_KEYWORD
};

#endif