#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// ANSI color codes
#define COLOR_RED     "\x1B[31m"
#define COLOR_GREEN   "\x1B[32m"
#define COLOR_YELLOW  "\x1B[33m"
#define COLOR_BLUE    "\x1B[34m"
#define COLOR_MAGENTA "\x1B[35m"
#define COLOR_CYAN    "\x1B[36m"
#define COLOR_RESET   "\x1B[0m"

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
    if (!(isalpha(str[0]) || str[0] == '_'))
        return failure;
    for (int i = 1; str[i] != '\0'; i++) {
        if (!(isalnum(str[i]) || str[i] == '_'))
            return failure;
    }
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
    printf(COLOR_RED "error: not a valid operator\n" COLOR_RESET);
    return error;
}

void lexical(lex_file *lex)
{
    char ch;
    char arr[MAX_TOKEN_SIZE];
    int count;
    // Bracket stack for balance checking
    char bracket_stack[1024];
    int top = -1;

    while ((ch = fgetc(lex->fptr)) != EOF) {
        // Skip whitespace
        if (isspace(ch)) continue;

        // Skip preprocessor directives
        if (ch == '#') {
            while (fgetc(lex->fptr) != '\n');
            continue;
        }

        // Track brackets inline
        if (ch == '(' || ch == '{' || ch == '[') {
            bracket_stack[++top] = ch;
        } else if (ch == ')' || ch == '}' || ch == ']') {
            if (top < 0) {
                printf(COLOR_RED "Unmatched closing bracket: %c\n" COLOR_RESET, ch);
            } else {
                char last = bracket_stack[top--];
                if ((ch == ')' && last != '(') ||
                    (ch == '}' && last != '{') ||
                    (ch == ']' && last != '[')) {
                    printf(COLOR_RED "Bracket mismatch: opened with %c but closed with %c\n" COLOR_RESET, last, ch);
                }
            }
        }

        // Numeric constants or invalid identifiers
        if (isdigit(ch)) {
            count = 0;
            arr[count++] = ch;
            char next = peek_next(lex->fptr);
            if (isalpha(next) || next == '_') {
                while ((ch = fgetc(lex->fptr)) != EOF && (isalnum(ch) || ch == '_')) {
                    arr[count++] = ch;
                }
                arr[count] = '\0';
                printf(COLOR_RED "Invalid identifier: %s\n" COLOR_RESET, arr);
                if (ch != EOF) ungetc(ch, lex->fptr);
                continue;
            }
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
            while ((ch = fgetc(lex->fptr)) != EOF && (isalnum(ch) || ch == '_')) {
                arr[count++] = ch;
            }
            arr[count] = '\0';
            if (ch != EOF) ungetc(ch, lex->fptr);
            if (check_if_keyword(arr) == success)
                printf("Keyword    : %s\n", arr);
            else if (isValidIdentifier(arr) == success)
                printf("Identifier : %s\n", arr);
            else
                printf("Invalid identifier: %s\n", arr);
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

    // Final bracket balance report
    if (top >= 0) {
        printf("Unmatched opening bracket(s) remain: ");
        while (top >= 0) {
            putchar(bracket_stack[top--]);
            if (top >= 0) putchar(' ');
        }
        putchar('\n');
    } else {
        printf("All brackets balanced.\n");
    }
}
