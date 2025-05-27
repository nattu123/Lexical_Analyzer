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
    "++", "--", "==", "!=", "<=", ">=", "+=", "-=", "*=", "/=", "%=", "&&", "||", ">>", "<<",
    "=", "+", "-", "*", "/", "%", "<", ">", "&", "|", "!", "^", "~"
};

static const char* specialCharacters = ",;{}()[]";

status initializeLexer(const char* filename, lex_file *lex)
{
    if (strstr(filename, ".") && strcmp(strstr(filename, "."), ".c") == 0) {
        printf("The input is a valid c file\n");
    } else {
        printf("Not a valid .c file\n");
        return failure;
    }
    lex->fptr = fopen(filename, "r");
    if (!lex->fptr) {
        printf("Could not open the c File\n");
        return failure;
    }
    fseek(lex->fptr, 0, SEEK_END);
    lex->file_size = ftell(lex->fptr);
    rewind(lex->fptr);
    printf("size of the file is %u\n", lex->file_size);
    return success;
}

char peek_next(FILE *fptr) {
    char c = fgetc(fptr);
    ungetc(c, fptr);
    return c;
}


// Check if a lexeme is a keyword
status check_if_keyword(char *arr) {
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        if (strcmp(arr, keywords[i]) == 0)
            return success;
    }
    return failure;
}

status isValidIdentifier(const char *str) {
    // Must start with a letter or underscore
    if (!(isalpha(str[0]) || str[0] == '_'))
        return failure;

    // Rest must be alnum or underscore
    for (int i = 1; str[i] != '\0'; i++) {
        if (!(isalnum(str[i]) || str[i] == '_'))
            return failure;
    }

    // Cannot be a keyword
    if (check_if_keyword((char*)str) == success)
        return failure;

    return success;
}



int isSpecialCharacter(char ch) {
    return strchr(specialCharacters, ch) != NULL;
}

char peek_next_char(FILE *fptr)
{
    char oper_chars[] = "=+-*/%<>&|!^~";
    char ch = fgetc(fptr);
    if (strchr(oper_chars, ch) == NULL) {
        ungetc(ch, fptr);
        return -1;
    }
    return ch;
}

status check_if_operator(char ch, FILE *fptr)
{
    char oper_chars[] = "=+-*/%<>&|!^~";
    char arr[5] = {0};
    int i = 0;

    if (!strchr(oper_chars, ch))
        return failure;

    do {
        if (ch == '/' && peek_next_char(fptr) == '/') {
            while (fgetc(fptr) != '\n');
            return success;
        }
        if (i >= 4) {
            printf("error: not a valid operator\n");
            return error;
        }
        arr[i++] = ch;
    } while ((ch = peek_next_char(fptr)) != -1);

    for (int j = 0; j < MAX_OPERATORS; j++) {
        if (strcmp(arr, operators[j]) == 0) {
            printf("Operator : %s\n", arr);
            return success;
        }
    }
    printf("error: not a valid operator\n");
    return error;
}

void lexical(lex_file *lex)
{
    char ch;
    char arr[MAX_TOKEN_SIZE];
    int count;

    while ((ch = fgetc(lex->fptr)) != EOF) {
        // Skip whitespace
        if (isspace(ch)) continue;

        // Skip preprocessor directives
        if (ch == '#') {
            while (fgetc(lex->fptr) != '\n');
            continue;
        }

    // Numeric constants
    if (isdigit(ch)) 
    {
        count = 0;
        arr[count++] = ch;
        char next = peek_next(lex->fptr);

        // If a letter or underscore follows a digit, it's not a valid number —
        // treat the whole run as an invalid identifier.
        if (isalpha(next) || next == '_') {
            while ((ch = fgetc(lex->fptr)) != EOF && (isalnum(ch) || ch == '_')) {
                arr[count++] = ch;
            }
            arr[count] = '\0';
            printf("Invalid identifier: %s\n", arr);
            if (ch != EOF) ungetc(ch, lex->fptr);
            continue;
        }

        // Otherwise, parse as a numeric constant (int or float)
        int dot_count = 0;
        while ((ch = fgetc(lex->fptr)) != EOF && (isdigit(ch) || ch == '.')) {
            if (ch == '.') dot_count++;
            arr[count++] = ch;
        }
        arr[count] = '\0';

        if (dot_count == 0)
            printf("Integer constant : %s\n", arr);
        else
            printf("Float constant   : %s\n", arr);

        if (ch != EOF) ungetc(ch, lex->fptr);
        continue;
}

            // Identifiers and keywords
    if (isalpha(ch) || ch == '_') {
        count = 0;
        arr[count++] = ch;
        // read the rest of the token
        while ((ch = fgetc(lex->fptr)) != EOF && (isalnum(ch) || ch == '_')) {
            arr[count++] = ch;
        }
        arr[count] = '\0';
        if (ch != EOF) ungetc(ch, lex->fptr);

        if (check_if_keyword(arr) == success) {
            printf("Keyword    : %s\n", arr);
        }
        else if (isValidIdentifier(arr) == success) {
            printf("Identifier : %s\n", arr);
        }
        else {
            printf("Invalid identifier: %s\n", arr);
        }
        continue;
    }

        // Operators
        if (check_if_operator(ch, lex->fptr) == success) {
            continue;
        }

        // Character and string literals
        if (ch == '\'' || ch == '"') {
            count = 0;
            char quote = ch;
            arr[count++] = ch;
            while ((ch = fgetc(lex->fptr)) != EOF && ch != quote) {
                arr[count++] = ch;
            }
            arr[count++] = ch;
            arr[count] = '\0';
            if (quote == '\'')
                printf("Character constant: %s\n", arr);
            else
                printf("String literal    : %s\n", arr);
            continue;
        }

        // Special characters
        if (isSpecialCharacter(ch)) {
            printf("Special character : %c\n", ch);
        } else {
            printf("Unknown token     : %c\n", ch);
        }
    }
}
