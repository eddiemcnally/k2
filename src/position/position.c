#include "position.h"

void set_cast_perm(cast_perm_t* cp, const enum castle_perm perm)
{
	if (perm == CAST_PERM_NONE) {
		*cp = CAST_PERM_NONE;
	} else {
		*cp = *cp | perm;
	}
}


void clear_cast_perm(cast_perm_t* cp, const enum castle_perm perm)
{
	*cp = cp & (~perm);
}
