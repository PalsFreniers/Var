#include "error.h"
#include <stdlib.h>
#include <stdio.h>

void panic(Panic_out_f func, void *data) {
        if(func) func(data);
        fprintf(stderr, "[[[PANIC]]] => Software panicked!!!\n");
        exit(255);
}
