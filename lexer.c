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

const char *operators[] = {

    ">>=", "<<=",

    "++", "--", "==", "!=", "<=", ">=", "+=", "-=", "*=", "/=", "%=","&&", "||", ">>", "<<",

    "=", "+", "-", "*", "/", "%", "<", ">", "&", "|", "!", "^", "~"
};

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



status check_if_keyword(char *arr)
{
    for(int i =0;i<MAX_KEYWORDS;i++)
    {
      if(strcmp(arr,keywords[i])==0) return success;
    }
    return failure;
}


// look at the next character 
char peek_next_chara(FILE *fptr)
{
    char *oper = "=+-*/%<>&|!^~";
    char ch = fgetc(fptr);
    
    if(strchr(oper,ch) == NULL)
    {
        ungetc(ch,fptr);
        return -1;
    }
    else
    {
        return ch;
    }
}

status check_if_operator(char ch,FILE *fptr)
{
    char *oper = "=+-*/%<>&|!^~";
    char arr[5];
    int i=0;
    // printf("--- %c ---",ch);
    if(strchr(oper,ch) == NULL)
    {
        return failure;
    }

    do
    {
        // ignore single line comments 
        if(ch == '/' && peek_next_chara(fptr) == '/')
        {
            while(fgetc(fptr)!= '\n');
            arr[i]= '\0';
            if(i==0) return success;
            else break;
        }
        // print error if the operator passes the threshold  
        if(i >= 4)
        {
            printf("error not a valid operator \n") ;
            return error;
        }

        // add operator to the array 
        arr[i++] = ch;
        arr[i] = '\0';
    }
    while((ch = peek_next_chara(fptr))!=-1);

    // compare with the given list of operators 
    for(int j=0; j<MAX_OPERATORS ; j++)
    {
        if(strcmp(arr,operators[j]) == 0)
        {
            printf("Operator : %s\n",arr);
            return success;
        }
    }

    printf("error not a valid operator ");
    return error;
}

void lexical(lex_file *lex)
{

    char ch;
    char arr[MAX_TOKEN_SIZE];
    int count =0; 
    while((ch = fgetc(lex->fptr)) != EOF)
    {

        // if preprocessor directiv edetected skip the whole line
        if(ch == '#')
        {
            while(fgetc(lex->fptr)!='\n');
            continue;
        }

        // if digit is detected and arr is 0 
        if((isdigit(ch)!=0 )&& count == 0)
        {
            arr[count++] = ch;
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
                        printf("float constant :%s\n",arr);
                    }
                    continue;
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
            else
            {
                printf("float constant :%s\n",arr);
            }
            continue;
        }

        if(isalnum(ch)!= 0) //check if alphanum, if true add it to the array 
        {
            if(count == 100)
            {
                printf("error : maximumm token size exceeded\n");
                return; 
            } 
            arr[count++] = ch;
        }
        else if(count != 0) // if not alphanum and array not empty , find if keyword ot identifier 
        {
            arr[count] = '\0';
            count =0;
            if(check_if_keyword(arr) == success) printf("keyword : %s\n",arr);
            else printf("Identifier : %s\n",arr); 
            ungetc(ch,lex->fptr);
        }
        else
        {
            if(ch ==' ' || ch == '\n') continue; 

            if(check_if_operator(ch,lex->fptr) == success);
            else if(ch == '\''  || ch =='\"') // check if character constant 
            {
                do
                {
                    arr[count++] = ch;
                }
                while((ch = fgetc(lex->fptr))!= '\'' && ch !='\"');
                arr[count++] = ch;
                arr[count] = '\0';
                if(ch=='\'')printf("character constant %s\n",arr);
                else if(ch=='\"')printf("string literal %s\n",arr);
                count =0;
            }
            else printf(" special character : %c\n",ch);

        }
    }
}