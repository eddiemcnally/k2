#include <stdint.h>
#include "position.h"

void add_cast_perm(cast_perm_t* cp, const enum castle_perm perm)
{
	if (perm == CAST_PERM_NONE) {
		*cp = CAST_PERM_NONE;
	} else {
		*cp = *cp | (uint8_t)perm;
	}
}


void remove_cast_perm(cast_perm_t* cp, const enum castle_perm perm)
{
	*cp = *cp & (~perm);
}
