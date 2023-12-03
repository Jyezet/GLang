#pragma once
#include <stdlib.h>
#include "../include/tokenlist.h"

#ifndef AST_H
#define AST_H

typedef struct ASTNode{
    ASTNode* parent;
    int num_children;
    int child_no;
    int type;
    char* content;
    ASTNode** children;
} ASTNode;

int countNodes(ASTNode*);
ASTNode newNode();
ASTNode newNode(TOKEN, const char*);
ASTNode* addNode(TOKEN, const char*);
ASTNode* addNode(ASTNode*);

#endif