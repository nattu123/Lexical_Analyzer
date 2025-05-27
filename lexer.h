#ifndef LEXER_H
#define LEXER_H
#define MAX_KEYWORDS 20
#define MAX_TOKEN_SIZE 100
#define MAX_OPERATORS 30 

typedef unsigned int uint;

typedef enum {
    KEYWORD,
    OPERATOR,
    SPECIAL_CHARACTER,
    CONSTANT,
    IDENTIFIER,
    Preprocessor_directive,
    UNKNOWN
} TokenType;



typedef struct{
    FILE *fptr;
    uint file_size;

}lex_file;
    

typedef enum 
{
    success,
    failure,
    error
}status;


status initializeLexer(const char* filename,lex_file *lex);
// Token getNextToken();
void lexical(lex_file *lex);
int isKeyword(const char* str);
int isOperator(const char* str);
int isSpecialCharacter(char ch);
int isConstant(const char* str);
int isIdentifier(const char* str);
status isValidIdentifier(const char *str);

#endif