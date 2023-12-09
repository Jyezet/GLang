#include <stdio.h>
#include <stdlib.h>
#include "Lexer/lexer.h"
#include "Parser/parser.h"
#include "Interpreter/interpreter.h"

/* --- TESTING FILE ONLY --- */
int main(){
    TokenHead* token_head = lexer_entryPoint("lexer/main.gl");

    for(Token* curr = token_head->first; curr != NULL; curr = curr->next){
        printf("Type: %i, content: %s\n", curr->type, curr->content);
    }

    //ParsedTree* parsed_tree = parser_entryPoint(token_head);
    //interpreter_entryPoint(parsed_tree);
    return 0;
}