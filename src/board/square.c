#include <assert.h>
#include "square.h"

static void validate_square(const enum square sq);


enum rank get_rank(const enum square sq)
{
	validate_square(sq);

	return (enum rank)(sq >> 3);
}

enum file get_file(const enum square sq)
{
	validate_square(sq);

	return (enum file)(sq % 8);
}

static inline void validate_square(const enum square sq)
{
#ifdef ENABLE_ASSERTS
	assert(sq >= a1 && sq <= h8);
#endif
}
