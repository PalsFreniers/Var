#include "strings.h"
#include "array.h"
#include "logger.h"
#include "error_code.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct Setup {
        bool valid;
        bool compiled;
        struct String source;
        struct String output;
        struct String entrypoint;
        struct String *arguments;
        struct String *module;
};

struct String findSetup() {
        char *pwd = getcwd(NULL, 0);
        DIR *dir = opendir(".");
        if(dir == NULL) {
                log_error("unable to open directory %s", pwd);
                free(pwd);
                return String_zero();
        }
        free(pwd);
        struct dirent *ent;
        struct String ret = String_zero();
        while((ent = readdir(dir)) != NULL) {
                if(ent->d_type == DT_REG && strcmp(ent->d_name, "setup.plsSetup") == 0) {
                        ret = String_newFromFile("setup.plsSetup");
                        break;
                }
        }
        closedir(dir);
        if(stringError != STRING_SUCCESS) log_error("String error : %s", StringError_toCStr());
        else if(ret.ptr == NULL) {
                log_error("Unable to find setup file in current directory");
                log_info("Be sure to create a setup.plsSetup in the current directory");
        }
        return ret;
}

union TokenValue {
        int i;
        str s;
        char c;
        bool b;
};

struct Token {
        int type;
        union TokenValue value;
};

struct Token *Token_New() {
        return calloc(1, sizeof(struct Token));
}

enum SetupToken {
        ST_BRACKET_OPEN,
        ST_BRACKET_CLOSE,
        ST_SQUARE_BRACKET_OPEN,
        ST_SQUARE_BRACKET_CLOSE,
        ST_IDENTIFIER,
        ST_COLON,
        ST_COMA,
};

void tokenFree(struct Token *self) {
        if(self->type == ST_IDENTIFIER) free(self->value.s);
        free(self);
}

struct DynamicArray lexSetup(struct String file) {
        struct DynamicArray ret = DynamicArray_new(10);
        for(struct StringIterator data = String_begin(&file);
                        !StringIterator_equal(data, String_end(&file));
                        StringIterator_inc(&data)) {
                struct Token *ptr;
                char c = StringIterator_peek(&data);
                if(c == '{') {
                        ptr = Token_New();
                        ptr->type = ST_BRACKET_OPEN;
                } else if(c == '}') {
                        ptr = Token_New();
                        ptr->type = ST_BRACKET_CLOSE;
                } else if(c == '[') {
                        ptr = Token_New();
                        ptr->type = ST_SQUARE_BRACKET_OPEN;
                } else if(c == ']') {
                        ptr = Token_New();
                        ptr->type = ST_SQUARE_BRACKET_CLOSE;
                } else if(isspace(c)) {
                        ptr = NULL;
                } else if(isalnum(c) || c == '_' || c == '.') {
                        struct String s = String_newWithCapacity(10);
                        while((isalnum(c) || c == '_' || c == '.') && stringError == STRING_SUCCESS) {
                                String_push(&s, c);
                                c = StringIterator_next(&data);
                        }
                        if(stringError != STRING_SUCCESS) {
                                log_error("Error during string creation : %s", StringError_toCStr());
                                String_destruct(&s);
                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)tokenFree);
                                return ret;
                        }
                        ptr = Token_New();
                        ptr->type = ST_IDENTIFIER;
                        ptr->value.s = String_toCstr(s);
                        String_destruct(&s);
                        StringIterator_dec(&data);
                } else if(c == ':') {
                        char c2 = StringIterator_next(&data);
                        if(c2 != ':') {
                                log_error("Error while lexing setup file : `:` character must be doubled, but founded `%c`", c2);
                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)tokenFree);
                                return ret;
                        }
                        ptr = Token_New();
                        ptr->type = ST_COLON;
                } else if(c == ',') {
                        ptr = Token_New();
                        ptr->type = ST_COMA;
                } else if(c == '#') {
                        int val = 0;
                        char n = StringIterator_next(&data);
                } else {
                        log_error("Error while lexing setup file : Unknown character `%c`", c);
                        DynamicArray_destruct(&ret, (DynamicArray_delete_f)tokenFree);
                        return ret;
                }
                if(ptr) DynamicArray_push(&ret, ptr);
        }
        return ret;
}

void printSetupTokens(struct Token *self) {
        switch (self->type) {
                case ST_BRACKET_OPEN:
                        log_debug("BRACKET_OPEN");
                        break;
                case ST_BRACKET_CLOSE:
                        log_debug("BRACKET_CLOSE");
                        break;
                case ST_SQUARE_BRACKET_OPEN:
                        log_debug("SQUARE_BRACKET_OPEN");
                        break;
                case ST_SQUARE_BRACKET_CLOSE:
                        log_debug("SQUARE_BRACKET_CLOSE");
                        break;
                case ST_IDENTIFIER:
                        log_debug("IDENTIFIER(%s)", self->value.s);
                        break;
                case ST_COLON:
                        log_debug("COLON");
                        break;
                case ST_COMA:
                        log_debug("COMA");
                        break;
                default:
                        log_debug("INVALID");
        }
}

struct Setup parseSetup(struct String file) {
        struct DynamicArray arr = lexSetup(file);
        if(arr.data == NULL) return (struct Setup) {0};
        DynamicArray_map(&arr, (DynamicArray_map_f)printSetupTokens);
        DynamicArray_destruct(&arr, (DynamicArray_delete_f)tokenFree);
        return (struct Setup) {0};
}

int main() {
        struct String setupF = findSetup();
        if(!setupF.ptr) return SETUP_NOT_FOUNDED_CODE;
        struct Setup setup = parseSetup(setupF);
        String_destruct(&setupF);
        if(!setup.valid) return INVALID_SETUP;
        return SUCCESS_CODE;
}
