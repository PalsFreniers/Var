#include "tokens.h"
#include <stdlib.h>

struct Token *Token_New() {
        return calloc(1, sizeof(struct Token));
}
