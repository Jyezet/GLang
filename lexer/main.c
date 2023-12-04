#include <stdio.h>
#include "lexer.h"
#include <stdlib.h>

/* --- TESTING FILE ONLY --- */
int main(){
    // TODO: Fix bug where first token is processed twice
    char code[] = "let var = \"code\";let var2 = \"code2\";print(\"code {} code {}\", [var, var2]);\n";
    Tokenizer* tok = (Tokenizer*) malloc(sizeof(Tokenizer));
    tok->start = code;

    TokenHead* head = parseTokens(tok);
    Token* curr = head->first;
    while(curr != head->last){
        printf("Type: %i, content: \"%s\"\n", curr->type, curr->content);
        curr = curr->next;
    }
    
    return 0;
}