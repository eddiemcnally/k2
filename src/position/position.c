#include <stdint.h>
#include <stdbool.h>
#include "position.h"

void add_cast_perm(uint8_t* cp, const enum castle_perm perm)
{
    if (perm == CAST_PERM_NONE) {
        *cp = CAST_PERM_NONE;
    } else {
        *cp = *cp | (uint8_t) perm;
    }
}

void remove_cast_perm(uint8_t* cp, const enum castle_perm perm)
{
    *cp = *cp & (~perm);
}

bool has_cast_perm(const uint8_t cp, const enum castle_perm perm)
{
    if (perm == CAST_PERM_NONE) {
        return (cp == 0);
    }
    return (cp & perm) != 0;
}
