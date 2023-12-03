#include "AbstractSyntaxTree.h"

#ifndef AST_CPP
#define AST_CPP

ASTNode newNode(){
    ASTNode node;
    node.type = (int) TOKEN::UNKNOWN_TOKEN;
    node.content = 0;
    node.parent = NULL;
    node.num_children = 0;
    node.child_no = -1;
    
    return node;
}

ASTNode::ASTNode(TOKEN _Type, const char* _Content) {
    type = (int) _Type;
    content = const_cast<char*>(_Content);
    children = (ASTNode**) malloc(sizeof(ASTNode*));
}

ASTNode::ASTNode(ASTNode& node) {
    type = node.type;
    content = node.content;

    children = node.children;
    parent = node.parent;
    num_children = node.num_children;
    child_no = node.child_no;

    node.parent->children[node.child_no] = this;
}

ASTNode* ASTNode::addNode(TOKEN _Type, const char* _Content){
    ASTNode* newNode = new ASTNode(_Type, _Content);
    newNode->parent = this;
    newNode->child_no = ++num_children;
    children = (ASTNode**) realloc(children, num_children * sizeof(ASTNode*));
    children[num_children - 1] = newNode;

    return newNode;
}

ASTNode* ASTNode::addNode(ASTNode* _Node){
    ASTNode* newNode = _Node;
    newNode->parent = this;
    newNode->child_no = ++num_children;
    children = (ASTNode**) realloc(children, num_children * sizeof(ASTNode*));
    children[num_children - 1] = newNode;
}

// Small shortcut to call the function above
int countNodes(ASTNode* node){
    int count = 0;
    if(node == NULL) return count;

    for(int i = 0; i < node->num_children; i++){
        count += countNodes(node->children[i]);
    }

    return count;
}

int ASTNode::getType(){
    return type;
}
char* ASTNode::getContent(){
    return content;
}

#endif