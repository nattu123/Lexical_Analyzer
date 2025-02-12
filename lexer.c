#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

static const char* operators = "+-*/%=!<>|&";
static const char* specialCharacters = ",;{}()[]";


status initializeLexer(const char* filename,lex_file *lex)
{
    // checking if argument is a valid c file or not 
    if(strstr(filename,".") && (strcmp(strstr(filename,"."),".c")==0))
    {
        printf("The input is a valid c file \n");
    }
    else
    {
        printf("Not a valid .c file \n");
        return failure;
    }
    // opening file 
    lex->fptr  = fopen(filename,"r");
    if (lex->fptr == NULL)
    {
        printf("Could not open the c File \n");
        return  failure;
    }
    else
    {
        printf("Opened %s succesfully\n",filename);
    }

    // finding the size of the file 
    fseek(lex->fptr,0,SEEK_END);
    lex->file_size = ftell(lex->fptr);
    printf("size of the file is %d",lex->file_size);
    return success; 
}

