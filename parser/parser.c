#include "parser.h"

#ifndef PARSER_CPP
#define PARSER_CPP

/*ASTNode* parseTree(Tokenizer& _Tok){
    ASTNode* root = new ASTNode(TOKEN::ROOT_NODE, "ROOT_NODE");
    ASTNode* expectNode = root; // Where to append new nodes
    ASTNode* newNode; // Where to receive the return value of identifyToken
    int expectedChildren;
    while(!isEndOfString(_Tok.currC)){
        newNode = identifyToken(_Tok);
        expectNode->addNode(newNode);

        if(newNode->type == (int) TOKEN::WHILE_STMT){
            expectNode = newNode;
            expectedChildren = 2; // The condition and the statement body
        }

        _Tok++;
    }
}*/

ASTNode* parse(){
    //ASTNode* rootASTNode = parseTree(tokens);
}

#endif