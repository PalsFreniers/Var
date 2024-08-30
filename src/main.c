#include "strings.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

struct Setup {
        struct String source;
        bool compiled;
        struct String entrypoint;
        struct String *arguments;
        struct String *module;
        struct String output;
};

struct String findSetup() {
        DIR *dir = opendir(".");
        struct dirent *ent;
        struct String ret = String_zero();
        while((ent = readdir(dir)) == NULL) {
                if(ent->d_type == DT_REG && strcmp(ent->d_name, "setup.plsSetup") == 0) 
                        parseSetup(String_newFromFile("setup.plsSetup"));
        }
        closedir(dir);
}

int main(int c, char **v) {
        struct String s = String_newFromFile("Makefile");
        printf(STRING_PRINTF_FORMAT "\n", STRING_PRINTF_ARGUMENTS(&s));
        String_destruct(&s);
        return 0;
}
