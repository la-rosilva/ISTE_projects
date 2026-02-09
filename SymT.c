#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lex.h"

#define MAX 20
#define TableLength 30

struct Symbol {
    char lexeme[MAX];
    int size;
    char type[MAX];
};

struct SymbolTable {
    struct Symbol sym;
    struct SymbolTable *next;
};

struct SymbolTable *TABLE[TableLength] = {NULL};

int HASH(char *str) {
    int hash = 0;
    for (int i = 0; str[i] != '\0'; i++)
        hash += str[i];
    return hash % TableLength;
}

int getSize(char *type) {
    if (strcmp(type, "int") == 0) return 4;
    if (strcmp(type, "float") == 0) return 4;
    if (strcmp(type, "double") == 0) return 8;
    if (strcmp(type, "char") == 0) return 1;
    if (strcmp(type, "long") == 0) return 8;
    if (strcmp(type, "void") == 0) return 0;
    return 0;
}

int SEARCH(char *str) {
    int idx = HASH(str);
    struct SymbolTable *temp = TABLE[idx];

    while (temp) {
        if (strcmp(temp->sym.lexeme, str) == 0)
            return 1;
        temp = temp->next;
    }
    return 0;
}

void INSERT(struct Symbol s) {
    if (SEARCH(s.lexeme))
        return;

    int idx = HASH(s.lexeme);

    struct SymbolTable *newNode =
        (struct SymbolTable *)malloc(sizeof(struct SymbolTable));

    newNode->sym = s;
    newNode->next = TABLE[idx];
    TABLE[idx] = newNode;
}

void Display() {
    printf("\nSymbol Table:\n");

    for (int i = 0; i < TableLength; i++) {
        struct SymbolTable *temp = TABLE[i];

        while (temp) {
            printf("Lexeme: %-10s Type: %-6s Size: %d\n",
                   temp->sym.lexeme,
                   temp->sym.type,
                   temp->sym.size);
            temp = temp->next;
        }
    }
}

int main() {
    preprocess();
    fp = fopen("preprocessed.txt", "r");
    nextChar();

    struct token t;
    char Type[50] = "";

    while (1) {
        t = getNextToken();

        if (strcmp(t.token_name, "EOF") == 0)
            break;

        if (!strcmp(t.token_name, "int") ||
            !strcmp(t.token_name, "float") ||
            !strcmp(t.token_name, "double") ||
            !strcmp(t.token_name, "char") ||
            !strcmp(t.token_name, "void") ||
            !strcmp(t.token_name, "long")) {

            strcpy(Type, t.token_name);
            continue;
        }

        if (!strcmp(t.token_name, "ID") && Type[0] != '\0') {
            struct Symbol s;

            strcpy(s.lexeme, t.lexeme);
            strcpy(s.type, Type);
            s.size = getSize(Type);

            INSERT(s);
            continue;
        }

        if (!strcmp(t.token_name, "ID") &&
            (!strcmp(t.lexeme, "printf") ||
             !strcmp(t.lexeme, "scanf"))) {

            struct Symbol s;

            strcpy(s.lexeme, t.lexeme);
            strcpy(s.type, "NULL");
            s.size = 0;

            INSERT(s);
        }

        if (!strcmp(t.token_name, ";"))
            Type[0] = '\0';
    }

    Display();
    return 0;
}
