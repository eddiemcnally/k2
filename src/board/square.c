#include <stdio.h>
#include <assert.h>
#include "square.h"

static void validate_square(const enum square sq);
#define MAX_SQUARE_TEXT 4
static char square_text[MAX_SQUARE_TEXT];

char arr[5];

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

enum square get_square(const enum rank rank, const enum file file)
{
    return (enum square)((rank << 3) + file);
}

char * print_square(const enum square sq)
{
    int file = get_file(sq);
    int rank = get_rank(sq);

    sprintf(square_text, "%c%c", ('a' + file), ('1' + rank));

    return square_text;
}

static inline void validate_square(const enum square sq)
{
#ifdef ENABLE_ASSERTS
    assert(sq >= a1 && sq <= h8);
#endif
}
