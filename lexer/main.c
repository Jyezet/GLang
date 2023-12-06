#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

/* --- TESTING FILE ONLY --- */
// TODO: Fix bug where a file not ending with newline overflows
int main(){
    TokenHead* head = lexer_entryPoint("main.gl");

    for(Token* curr = head->first; curr != head->last; curr = curr->next){
        printf("Type: %i, content: %s\n", curr->type, curr->content);
    }
    
    return 0;
}