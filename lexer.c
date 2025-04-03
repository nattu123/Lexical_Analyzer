#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

#define set 1
#define clear 0



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
    rewind(lex->fptr);
    printf("size of the file is %d\n",lex->file_size);
    return success; 
}


// Token getNextToken(lex_file *lex)
// {
//     int i = 0;
//     char arr[100];
//     Token next_char;
//     char ch = fgetc(lex->fptr);

//     // detect preprocessor directives 
    
//     if(ch == '#')
//     {
//         fscanf(lex->fptr,"%[^\n]",arr+1);
//         printf("preprocessor directive detected \n");
//         arr[0] = ch;
//         strcpy(next_char.lexeme,arr);
//         next_char.type = KEYWORD;
//         return next_char;
//     }
    
//     while(strchr(specialCharacters,ch) == NULL &&  strchr(operators,ch) == NULL)
//     {
//         if(ch == '\n') break;
//         arr[i] = ch;
//         ch =fgetc(lex->fptr);
//         i++;
//     }

//     arr[i]='\0';
//     strcpy(next_char.lexeme,arr);

//     return next_char;  
    
// }

void lexical(lex_file *lex)
{
    char ch;
    char arr[MAX_TOKEN_SIZE];
    int count =0; 
    while((ch = fgetc(lex->fptr)) != EOF)
    {
        if(ch == '#')
        {
            while(fgetc(lex->fptr)!='\n');
            continue;
        }

        if((isdigit(ch)!=0 )&& count == 0)
        {
            int flag = 0;
            while((ch = fgetc(lex->fptr)) != ';')
            {
                if(ch == ',')
                {
                    arr[count] = '\0';
                    count = 0;
                    if(flag == 0)
                    {
                        printf("Integer constant : %s\n",arr);
                    }
                    else
                    {
                        printf("float constant :%s\n,arr");
                    }
                }

                if(ch == '.')flag++; 
                arr[count++] = ch;
            }
            arr[count] = '\0';
            count = 0; 
            if(flag == 0)
            {
                printf("Integer constant : %s\n",arr);
            }
        }
        if(isalpha)
    }
}