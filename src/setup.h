#ifndef SETUP_H
#define SETUP_H

#include "strings.h"

enum SetupToken {
        ST_BRACKET_OPEN,
        ST_BRACKET_CLOSE,
        ST_SQUARE_BRACKET_OPEN,
        ST_SQUARE_BRACKET_CLOSE,
        ST_IDENTIFIER,
        ST_COLON,
        ST_COMA,
        ST_ARGUMENT_NUMBER,
        ST_STRING,
        ST_END,
};

struct Setup {
        bool valid;
        bool compiled;
        struct String source;
        char *output;
        char *entrypoint;
        struct Token *arguments;
        char **modules;
};

struct String findSetup();

#endif // SETUP_H
