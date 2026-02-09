#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct token {
    char token_name[50];
    char lexeme[50];
    unsigned int row, col;
};

FILE *fp;
int c;

unsigned int row = 1, col = 0;
unsigned int token_start_row, token_start_col;

void nextChar() {
    c = fgetc(fp);
    if (c == '\n') {
        row++;
        col = 0;
    } else {
        col++;
    }
}

const char *keywords[] = {
    "int", "float", "double", "char", "void",
    "if", "else", "for", "while", "do",
    "break", "continue", "return",
    "switch", "case", "default",
    "struct", "typedef", "enum",
    "const", "static", "sizeof"
};

const char special_symbols[] = {
    ';', ',', '(', ')', '{', '}', '[', ']'
};

void preprocess() {
    FILE *in = fopen("input.c", "r");
    FILE *out = fopen("preprocessed.txt", "w");

    if (!in || !out) return;

    int ch;
    int inString = 0;
    int lastWasSpace = 0;

    while ((ch = fgetc(in)) != EOF) {

        if (ch == '"' && !inString) {
            inString = 1;
            fputc(ch, out);
            lastWasSpace = 0;
            continue;
        } 
        else if (ch == '"' && inString) {
            inString = 0;
            fputc(ch, out);
            lastWasSpace = 0;
            continue;
        }

        if (inString) {
            if (ch == '\\') {
                fputc(ch, out);
                ch = fgetc(in);
                fputc(ch, out);
                continue;
            }
            fputc(ch, out);
            continue;
        }

        if (ch == '#') {
            while (ch != '\n' && ch != EOF)
                ch = fgetc(in);
            fputc('\n', out);
            lastWasSpace = 0;
            continue;
        }

        if (ch == '/') {
            int next = fgetc(in);

            if (next == '/') {
                while (ch != '\n' && ch != EOF)
                    ch = fgetc(in);
                fputc('\n', out);
                lastWasSpace = 0;
                continue;
            }

            if (next == '*') {
                while ((ch = fgetc(in)) != EOF) {
                    if (ch == '\n')
                        fputc('\n', out);
                    if (ch == '*') {
                        int temp = fgetc(in);
                        if (temp == '/')
                            break;
                        else
                            ungetc(temp, in);
                    }
                }
                lastWasSpace = 0;
                continue;
            }

            fputc('/', out);
            ungetc(next, in);
            lastWasSpace = 0;
            continue;
        }

        if (ch == ' ' || ch == '\t') {
            if (!lastWasSpace) {
                fputc(' ', out);
                lastWasSpace = 1;
            }
            continue;
        }

        if (ch == '\n') {
            fputc('\n', out);
            lastWasSpace = 0;
            continue;
        }

        fputc(ch, out);
        lastWasSpace = 0;
    }

    fclose(in);
    fclose(out);
}

struct token makeToken(char *lexeme) {
    struct token t;
    strcpy(t.token_name, lexeme);
    t.row = token_start_row;
    t.col = token_start_col;
    return t;
}

struct token arithmetic() {
    char buf[5];
    int i = 0;

    buf[i++] = c;
    nextChar();

    if ((buf[0] == '+' && (c == '+' || c == '=')) ||
        (buf[0] == '-' && (c == '-' || c == '=')) ||
        (buf[0] == '*' && c == '=') ||
        (buf[0] == '/' && c == '=') ||
        (buf[0] == '%' && c == '=')) {

        buf[i++] = c;
        nextChar();
    }

    buf[i] = '\0';
    return makeToken(buf);
}

struct token relational() {
    char buf[3];
    int i = 0;

    buf[i++] = c;
    nextChar();

    if ((buf[0] == '<' || buf[0] == '>' || buf[0] == '=' || buf[0] == '!') && c == '=') {
        buf[i++] = c;
        nextChar();
    }

    buf[i] = '\0';
    return makeToken(buf);
}

struct token logical() {
    char buf[3];
    int i = 0;

    buf[i++] = c;
    nextChar();

    if ((buf[0] == '&' && c == '&') || (buf[0] == '|' && c == '|')) {
        buf[i++] = c;
        nextChar();
    }

    buf[i] = '\0';
    return makeToken(buf);
}

struct token spsymb() {
    char buf[2];

    buf[0] = c;
    buf[1] = '\0';

    nextChar();
    return makeToken(buf);
}

struct token num_const() {
    char buf[50];
    int i = 0;

    while (isdigit(c) || c == '.') {
        buf[i++] = c;
        nextChar();
    }

    buf[i] = '\0';
    return makeToken(buf);
}

struct token str_literal() {
    char buf[200];
    int i = 0;

    buf[i++] = c;
    nextChar();

    while (c != '"' && c != EOF) {
        if (c == '\\') {
            buf[i++] = c;
            nextChar();
        }
        buf[i++] = c;
        nextChar();
    }

    if (c == '"') {
        buf[i++] = c;
        nextChar();
    }

    buf[i] = '\0';
    return makeToken(buf);
}

struct token str_identifier() {
    char buf[50];
    int i = 0;

    while (isalnum(c) || c == '_') {
        buf[i++] = c;
        nextChar();
    }

    buf[i] = '\0';

    struct token t;

    int n = sizeof(keywords) / sizeof(keywords[0]);
    for (int j = 0; j < n; j++) {
        if (strcmp(buf, keywords[j]) == 0) {
            strcpy(t.token_name, buf);
            strcpy(t.lexeme, buf);
            t.row = token_start_row;
            t.col = token_start_col;
            return t;
        }
    }

    strcpy(t.token_name, "ID");
    strcpy(t.lexeme, buf);
    t.row = token_start_row;
    t.col = token_start_col;
    return t;
}

struct token getNextToken() {
    while (c == ' ' || c == '\t' || c == '\n')
        nextChar();

    if (c == EOF) {
        struct token t;
        strcpy(t.token_name, "EOF");
        t.row = row;
        t.col = col;
        return t;
    }

    token_start_row = row;
    token_start_col = col;

    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
        return arithmetic();

    if (c == '<' || c == '>' || c == '=' || c == '!')
        return relational();

    if (c == '&' || c == '|')
        return logical();

    for (int i = 0; i < sizeof(special_symbols) / sizeof(special_symbols[0]); i++) {
        if (c == special_symbols[i])
            return spsymb();
    }

    if (c == '"')
        return str_literal();

    if (isdigit(c))
        return num_const();

    if (isalpha(c) || c == '_')
        return str_identifier();

    nextChar();
    return makeToken("UNKNOWN");
}
