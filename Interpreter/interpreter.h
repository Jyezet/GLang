#ifndef INTERPRETER_H
#define INTERPRETER_H

#pragma once
#include "../Parser/syntax_tree.h"
#include "data_structs.h"

double parseNumber(const char* _Text, int base){
    // TODO: Scientific notation
    double number = 0;
    int decimal = 0;
    int decimal_pos = 1;
    for(int i = 0; !isEndOfString(_Text[i]); i++){
        if(isDigit(_Text[i])){
            number = number * base + _Text[i] - '0';
            if(decimal) { decimal_pos *= base; }
        } else if(_Text[i] == '-' && i != 0){
            THROW_EXC_INFO("'-' symbol can only be at the beginning of a number.", _Text);
        } else if(_Text[i] == '.'){
            decimal = 1;
        } else {
            THROW_EXC_INFO("Non numeric character", _Text[i]);
        }
    }
    
    if(_Text[0] == '-') { number *= -1; }
    number /= decimal_pos;
    return number;
}

Expr* evalExpr(Expr* expression){
    void* left_side;
    void* right_side;
    void* temp;
    
    int left_side_type;
    int right_side_type;

    if(expression->left_right_side[0]->mode == FINAL_EXPR){
        left_side_type = expression->left_right_side[0]->number->type;

        if(left_side_type == DOUBLE_NUMBER){
            left_side = malloc(sizeof(double));
            *(double*) left_side = expression->left_right_side[0]->number->d_num;
        } else {
            left_side = malloc(sizeof(int));
            *(int*) left_side = expression->left_right_side[0]->number->i_num;
        }
    } else {
        left_side = evalExpr(expression->left_right_side[0]);

        if(((ArithmNumber*) left_side)->type == DOUBLE_NUMBER){
            temp = left_side; // Avoid losing reference to old left side
            left_side = malloc(sizeof(double));
            *(double*) left_side = ((ArithmNumber*) temp)->d_num;
            free(temp);
        } else {
            temp = left_side;
            left_side = malloc(sizeof(int));
            *(int*) left_side = ((ArithmNumber*) temp)->i_num;
            free(temp);
        }
    }

    if(expression->left_right_side[1]->mode == FINAL_EXPR){
        right_side_type = expression->left_right_side[1]->number->type;

        if(right_side_type == DOUBLE_NUMBER){
            right_side = malloc(sizeof(double));
            *(double*) right_side = expression->left_right_side[1]->number->d_num;
        } else {
            right_side = malloc(sizeof(int));
            *(int*) right_side = expression->left_right_side[1]->number->i_num;
        }
    } else {
        right_side = evalExpr(expression->left_right_side[1]);

        if(((ArithmNumber*) right_side)->type == DOUBLE_NUMBER){
            temp = right_side; // Avoid losing reference to old right side
            right_side = malloc(sizeof(double));
            *(double*) right_side = ((ArithmNumber*) temp)->d_num;
            free(temp);
        } else {
            temp = right_side;
            right_side = malloc(sizeof(int));
            *(int*) right_side = ((ArithmNumber*) temp)->i_num;
            free(temp);
        }
    }

    ArithmNumber* numeric_result = (ArithmNumber*) malloc(sizeof(ArithmNumber));
    Expr* result = (Expr*) malloc(sizeof(Expr));
    result->mode = FINAL_EXPR;
    result->number = numeric_result;

    switch(expression->op){
        case '+':
            if(left_side_type == DOUBLE_NUMBER || right_side_type == DOUBLE_NUMBER){
                numeric_result->type = DOUBLE_NUMBER;
                numeric_result->d_num = *(double*) left_side + *(double*) right_side;
                break;
            }

            numeric_result->type = INTEGER_NUMBER;
            numeric_result->d_num = *(int*) left_side + *(int*) right_side;
            break;

        case '-':
            if(left_side_type == DOUBLE_NUMBER || right_side_type == DOUBLE_NUMBER){
                numeric_result->type = DOUBLE_NUMBER;
                numeric_result->d_num = *(double*) left_side - *(double*) right_side;
                break;
            }

            numeric_result->type = INTEGER_NUMBER;
            numeric_result->d_num = *(int*) left_side - *(int*) right_side;
            break;

        case '*':
            if(left_side_type == DOUBLE_NUMBER || right_side_type == DOUBLE_NUMBER){
                numeric_result->type = DOUBLE_NUMBER;
                numeric_result->d_num = *(double*) left_side * *(double*) right_side;
                break;
            }

            numeric_result->type = INTEGER_NUMBER;
            numeric_result->d_num = *(int*) left_side * *(int*) right_side;
            break;

        case '/':
            if((int) right_side == 0){
                THROW_EXC("Division by zero exception");
            }

            if(left_side_type == DOUBLE_NUMBER || right_side_type == DOUBLE_NUMBER){
                numeric_result->type = DOUBLE_NUMBER;
                numeric_result->d_num = *(double*) left_side / *(double*) right_side;
                break;
            }

            numeric_result->type = INTEGER_NUMBER;
            numeric_result->d_num = *(int*) left_side / *(int*) right_side;
            break;
        default:
            THROW_EXC_INFO("Unknown arithmetical operator", expression->op);
    }

    free(left_side);
    free(right_side);
    free(temp);
    return result;
}

void interpreter_entryPoint(ParsedTree* tree){

}

#endif