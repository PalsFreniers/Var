#include "setup.h"
#include <dirent.h>
#include "logger.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "tokens.h"

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

void setupTokenFree(struct Token *self) {
        if(self->type == ST_IDENTIFIER || self->type == ST_STRING) free(self->value.s);
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
                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
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
                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
                                return ret;
                        }
                        ptr = Token_New();
                        ptr->type = ST_COLON;
                } else if(c == ',') {
                        ptr = Token_New();
                        ptr->type = ST_COMA;
                } else if(c == '#') {
                        long val = 0;
                        char n = StringIterator_next(&data);
                        struct String s = String_newWithCapacity(10);
                        while(isdigit(n)) {
                                String_push(&s, n);
                                n = StringIterator_next(&data);
                        }
                        if(stringError != STRING_SUCCESS) {
                                log_error("Error during string creation : %s", StringError_toCStr());
                                String_destruct(&s);
                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
                                return ret;
                        }
                        if(s.len == 0) {
                                log_error("Error while lexing setup file : unable to find argv number");
                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
                                String_destruct(&s);
                                return ret;
                        }
                        char *number = String_toCstr(s);
                        String_destruct(&s);
                        if(strlen(number) > 10 || (val = atol(number)) > INT_MAX) {
                                log_error("Error while lexing setup file : argv number too high `#%s` (max `%d`)", number, INT_MAX);
                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
                                free(number);
                                return ret;
                        }
                        free(number);
                        ptr = Token_New();
                        ptr->type = ST_ARGUMENT_NUMBER;
                        ptr->value.i = val;
                } else if(c == '"') {
                        struct String s = String_newWithCapacity(10);
                        c = StringIterator_next(&data);
                        while(c != '"' && stringError == STRING_SUCCESS) {
                                if(c == '\\') {
                                        c = StringIterator_next(&data);
                                        if(stringError != STRING_SUCCESS) {
                                                log_error("Error during string creation : %s", StringError_toCStr());
                                                String_destruct(&s);
                                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
                                                return ret;
                                        }
                                        switch(c) {
                                                case 'a':
                                                        c = '\a';
                                                        break;
                                                case 'b':
                                                        c = '\b';
                                                        break;
                                                case 'f':
                                                        c = '\f';
                                                        break;
                                                case 'r':
                                                        c = '\r';
                                                        break;
                                                case 'n':
                                                        c = '\n';
                                                        break;
                                                case 't':
                                                        c = '\t';
                                                        break;
                                                case 'v':
                                                        c = '\v';
                                                        break;
                                                case '"':
                                                        c = '"';
                                                        break;
                                                default:
                                                        log_error("Error unable to find character : `\\%c`", c);
                                                        String_destruct(&s);
                                                        DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
                                                        return ret;
                                        }
                                }
                                String_push(&s, c);
                                c = StringIterator_next(&data);
                        }
                        if(stringError != STRING_SUCCESS) {
                                log_error("Error during string creation : %s", StringError_toCStr());
                                String_destruct(&s);
                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
                                return ret;
                        }
                        if(s.len == 0) {
                                log_error("Error empty string");
                                String_destruct(&s);
                                DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
                                return ret;
                        }
                        char *d = String_toCstr(s);
                        String_destruct(&s);
                        ptr = Token_New();
                        ptr->type = ST_STRING;
                        ptr->value.s = d;
                } else {
                        log_error("Error while lexing setup file : Unknown character `%c`", c);
                        DynamicArray_destruct(&ret, (DynamicArray_delete_f)setupTokenFree);
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
                case ST_ARGUMENT_NUMBER:
                        log_debug("ARGUMENT(%d)", self->value.i);
                        break;
                case ST_STRING:
                        log_debug("STRING(%s)", self->value.s);
                        break;
                default:
                        log_debug("INVALID");
        }
}

char *getSetupTokens(enum SetupToken type) {
        switch (type) {
                case ST_BRACKET_OPEN:
                        return "BRACKET_OPEN";
                case ST_BRACKET_CLOSE:
                        return "BRACKET_CLOSE";
                case ST_SQUARE_BRACKET_OPEN:
                        return "SQUARE_BRACKET_OPEN";
                case ST_SQUARE_BRACKET_CLOSE:
                        return "SQUARE_BRACKET_CLOSE";
                case ST_IDENTIFIER:
                        return "IDENTIFIER";
                case ST_COLON:
                        return "COLON";
                case ST_COMA:
                        return "COMA";
                case ST_ARGUMENT_NUMBER:
                        return "ARGUMENT";
                case ST_STRING:
                        return "STRING";
                default:
                        return"INVALID";
        }
}

void setupParseError(char *fmt) {
        log_error("Error during setup parsing : %s", fmt);
}

void setupParseErrorUnexpectedToken(enum SetupToken t1, enum SetupToken t2) {
        log_error("Error during setup parsing : Unexpected token `%s` (expected `%s`)", getSetupTokens(t1), getSetupTokens(t2));
}

void setupFree(struct Setup *self) {
        if(self->arguments) {
                for(int i = 0; self->arguments[i].type != ST_END; i++) setupTokenFree(&(self->arguments[i]));
                free(self->arguments);
        }
        if(self->modules) {
                for(int i = 0; self->modules[i] != NULL; i++) free(self->modules);
                free(self->modules);
        }
        if(self->entrypoint) free(self->entrypoint);
        if(self->output) free(self->output);
        String_destruct(&self->source);
        self->valid = false;
}

bool strsuffix(char *hay, char *needle) {
        if(!hay || !needle) return false;
        int l1 = strlen(hay);
        int l2 = strlen(needle);
        if(l1 < l2) return false;
        return strcmp(hay + l1 - l2, needle) == 0;
}

bool parseSetupSource(struct DynamicArrayIterator *iter, struct Setup *setup) {
        bool ret = true;
        struct Token *curr = DynamicArrayIterator_next(iter);
        if(dynamicArrayError != DYNAMIC_ARRAY_SUCCESS || curr->type != ST_BRACKET_OPEN) {
                setupParseErrorUnexpectedToken(curr->type, ST_BRACKET_OPEN);
                ret = false;
                goto end;
        }
        curr = DynamicArrayIterator_next(iter);
        if(dynamicArrayError != DYNAMIC_ARRAY_SUCCESS || curr->type != ST_IDENTIFIER) {
                setupParseErrorUnexpectedToken(curr->type, ST_IDENTIFIER);
                ret = false;
                goto end;
        }
        if(!strsuffix(curr->value.s, ".plsFile")) {
                setupParseError("file does not have extension `.plsFile`");
                ret = false;
                goto end;
        }
        curr = DynamicArrayIterator_next(iter);
        if(dynamicArrayError != DYNAMIC_ARRAY_SUCCESS || curr->type != ST_IDENTIFIER) {
                setupParseErrorUnexpectedToken(curr->type, ST_IDENTIFIER);
                ret = false;
                goto end;
        }

end:
        return ret;
}

struct Setup parseSetup(struct String file) {
        struct DynamicArray arr = lexSetup(file);
        if(arr.data == NULL) return (struct Setup) {0};
        struct DynamicArrayIterator iter = DynamicArray_begin(&arr);
        struct Setup s = {0};
        if(!parseSetupSource(&iter, &s)) goto errRet;
        goto ret;
errRet:
        setupFree(&s);
ret:
        DynamicArray_destruct(&arr, (DynamicArray_delete_f)tokenFree);
        return s;
}
