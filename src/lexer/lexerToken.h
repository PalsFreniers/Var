#pragma once

#define MAX_TOKEN_STRING 10000

enum LexerTokenType {
    NONE,
    // litterals
    BOOLEAN_LITTERALS,
    NUMBER_LITTERALS,
    CHARACTER_LITTERALS,
    STRING_LITTERALS,
    // operators
    MATH_OPERATORS,
    BOOLEAN_OPERATOR,
    CASTING_OPERATOR,
    // left
    IDENTIFIER,
    KEYWORD,
    ARGUMENTS,
};

typedef struct LexerToken {
    enum LexerTokenType tokenType;
    char tokenString[MAX_TOKEN_STRING];
} LexerToken_t;

LexerToken_t createLexerToken(enum LexerTokenType, char str[MAX_TOKEN_STRING]);

typedef struct LexerTokenList {
    LexerToken_t currentToken;
    struct LexerTokenList* next;
    int index;
} LexerTokenList_t;

LexerTokenList_t* createTokenListHead();
void appendLexerToken(LexerTokenList_t* head, LexerToken_t token);
LexerTokenList_t* popLexerToken(LexerTokenList_t* head);
void destructLexerTokenList(LexerTokenList_t* head);
