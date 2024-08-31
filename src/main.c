#include "strings.h"
#include "array.h"
#include "logger.h"
#include "error_code.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "setup.h"


int main() {
        struct String setupF = findSetup();
        if(!setupF.ptr) return SETUP_NOT_FOUNDED_CODE;
        struct Setup setup = parseSetup(setupF);
        String_destruct(&setupF);
        if(!setup.valid) return INVALID_SETUP;
        return SUCCESS_CODE;
}
