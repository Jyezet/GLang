#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H

enum ARITHM{
    DOUBLE_NUMBER,
    INTEGER_NUMBER,
    OPERATION_EXPR, // An expression which points to other 2 expressions
    FINAL_EXPR // An expression which points to a number, the end of recursion
};

enum STMT_TYPE{
    /* --- Variables and constants --- */
    VAR_DECL, // let a;
    VAR_SET, // a = 2;
    VAR_DEF, // let a = 2;
    CONST_DECL, // const b; (Uninitialized constant warning)
    CONST_SET, // b = 4; (Assignation to constant exception)
    CONST_DEF, // const b = 4;

    /* --- rvalues --- */
    ARITHM_OP,
    BOOLEAN_OP,
    ELEM_LIST, // a, b, c (Any set of identifiers separated by commas, in any context)

    /* --- Statements --- */
    WHILE,
    FOR,
    IF,
    ELSE,
    
    /* --- Functions --- */
    FUNC_DECL, // fn func();
    FUNC_SET, // func() -> { STATEMENTS }
    FUNC_DEF, // fn func() -> { STATEMENTS }

    FUNC_DECL_NO_ARGLIST, // fn func; (Undefined argument list exception)
    FUNC_SET_NO_ARGLIST, // func -> { STATEMENTS } (Undefined argument list exception)
    FUNC_DEF_NO_ARGLIST, // fn func -> { STATEMENTS } (Undefined argument list exception)
    FUNC_CALL // func();
};

typedef struct{
    int type;
    union{
        double d_num;
        int i_num;
    };
} ArithmNumber;

typedef struct Expr{
    int mode;
    char op;
    union{
        ArithmNumber* number;
        struct Expr** left_right_side;
    };
} Expr;

typedef struct Statement{
    struct Statement* prev_stmt;
    struct Statement* next_stmt;
    int stmt_type;
} Statement;

typedef struct{
    Statement* first_stmt;
    Statement* last_stmt;
    int symbols;
} ParsedTree;

#endif