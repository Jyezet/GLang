#include <stdio.h>
#include "lexer.h"
#include <stdlib.h>

/* --- TESTING FILE ONLY --- */
int main(){
    char code[] = "let var = \"code\"; let var2 = \"code2\";\nprint(\"code {} code {}\", [$var, $var2]);";
    Tokenizer* tok = (Tokenizer*) malloc(sizeof(Tokenizer));
    tok->start = code;

    Token* head = parseTokens(tok);
    Token* curr;
    while(curr->next != NULL){
        printf("Type: %i, content: %s\n", curr->type, curr->content);
        curr = curr->next;
    }

    return 0;
}