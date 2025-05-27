#include <stdio.h>
#include "lexer.h"

int main(int argc, char *argv[]) 
{

    lex_file lex;
    if(initializeLexer(argv[1],&lex)== failure) return failure; 
    lexical(&lex); 
    return 0;
}   