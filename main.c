#include <stdio.h>
#include "lexer.h"

int main(int argc, char *argv[]) 
{

    lex_file lex;
    if(initializeLexer(argv[1],&lex)== failure) return failure; 

    // Token token;
    
    // printf("\n\n\n");
    // while ((token = getNextToken(&lex)).type != UNKNOWN) {
    //     printf("Token: %s, Type: %d\n", token.lexeme, token.type);
    // }
    lexical(&lex); 
    return 0;
}   