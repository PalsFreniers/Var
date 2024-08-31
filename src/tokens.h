#ifndef TOKEN_H
#define TOKEN_H

#include <stdbool.h>

union TokenValue {
        int i;
        char *s;
        char c;
        bool b;
};

struct Token {
        int type;
        union TokenValue value;
};

struct Token *Token_New();

#endif // TOKEN_H
