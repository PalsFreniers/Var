#include "lexerToken.h"
#include <stdlib.h>

LexerTokenList_t *createTokenListHead() {
    LexerTokenList_t* ret = malloc(sizeof(LexerTokenList_t));
    ret->currentToken = createLexerToken(NONE, "");
    ret->next = NULL;
    ret->index = 0;
    return ret;
}

