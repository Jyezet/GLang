#include <stdio.h>
#include "lexer.h"
#include <stdlib.h>

/* --- TESTING FILE ONLY --- */
// TODO: Fix bug where string literal is split in half
// TODO: Fix bug where multi-line comment is not detected
int main(){
    char code[] = "let var = 4 -2; /* test */ let var2 = \" code2 \"; print(\"code {} code {}\", [var, var2]);\n";
    Tokenizer* tok = (Tokenizer*) malloc(sizeof(Tokenizer));
    tok->start = code;

    TokenHead* head = parseTokens(tok, code + strlen(code));
    Token* curr = head->first;
    while(curr != head->last){
        printf("Type: %i, content: \"%s\"\n", curr->type, curr->content);
        curr = curr->next;
    }
    
    return 0;
}