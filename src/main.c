#include <stdio.h>
#include <stdlib.h>

long fileSize(FILE* f) {
    if(!f) return 0;
    fpos_t out;
    long ret = 0;
    if(fgetpos(f, &out)){
        printf("Error : Unable to get file cursor position\n");
        return 0;
    }
    fseek(f, 0, SEEK_END);
    ret = ftell(f);
    if(fsetpos(f, &out)){
        printf("Error : Unable to reset file cursor position\n");
        return 0;
    }
    return ret;
}

char* fileContent(char* path) {
    FILE* f = fopen(path, "r");
    if(!f) {
        printf("Error : Unable to open file %s verify if it exist\n", path);
        exit(1);
    }
    long fSize = fileSize(f);
    char* code = malloc(fSize+1);
    long bytes = 0;
    size_t it = 0;
    do {
        it = fread(code, 1, fSize - bytes, f);
        bytes += it;
    } while(it != 0);
    code[fSize] = '\0';
    if(bytes != fSize) {
        printf("Error : file size  %ld does not correspond to file byte number %ld\n", fSize, bytes);
        fclose(f);
        free(code);
        exit(1);
    }
    fclose(f);
    return code;
}

void usage(char* ProgramName) {
    printf("USAGE: %s <PATH_TO_SETUP_FILE>\n", ProgramName);
    exit(1);
}

int main(int argc, char** argv) {
    if(argc < 2) usage(argv[0]);
    char* test = fileContent(argv[1]);
    printf("%s", test);
    free(test);
    return 0;
}
