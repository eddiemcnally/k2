/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without 
 *  restriction, including without limitation the rights to use, 
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the 
 *  Software is furnished to do so, subject to the following 
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS 
 *  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
 *  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*! @addtogroup Square
 *
 * @ingroup Square
 * @{
 * @details A set of enums and functions associated with manipulating Sqaures, Ranks and Files.
 *
 */

#include "square.h"
#include <assert.h>

// Temp storage when generating a string representation of a square
#define MAX_SQUARE_TEXT (4)
static char square_text[MAX_SQUARE_TEXT];

// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief       Returns the rank for the given square
 * @param sq    The square
 * @return      The rank for the given square
 */
enum rank sq_get_rank(enum square sq) {
    assert(validate_square(sq));

    return (enum rank)(sq >> 3);
}

/**
 * @brief       Returns the file for the given square
 * @param sq    The square
 * @return      The file for the given square
 */
enum file sq_get_file(enum square sq) {
    assert(validate_square(sq));

    // efficient mod operation....
    //      x mod y == x & y-1
    // works when y is a power of 2
    //
    // and file is (sq % 8)
    return (enum file)(sq & 7);
}

/**
 * @brief       Returns the square that is up one rank from the given square
 * @param sq    The square
 * @return      The square that is 1 rank up
 */
enum square sq_get_square_plus_1_rank(enum square sq) {
    assert(validate_square(sq));
    assert(sq_get_rank(sq) <= RANK_7);
    return (enum square)(sq + 8);
}

/**
 * @brief       Returns the square that is down one rank from the given square
 * @param sq    The square
 * @return      The square that is 1 rank down
 */
enum square sq_get_square_minus_1_rank(enum square sq) {
    assert(validate_square(sq));
    assert(sq_get_rank(sq) >= RANK_2);

    return (enum square)(sq - 8);
}

enum square sq_get_square_plus_2_rank(enum square sq) {
    assert(validate_square(sq));
    assert(sq_get_rank(sq) <= RANK_6);

    return (enum square)(sq + 16);
}
enum square sq_get_square_minus_2_rank(enum square sq) {
    assert(validate_square(sq));
    assert(sq_get_rank(sq) >= RANK_3);

    return (enum square)(sq - 16);
}

bool sq_is_valid_rank(enum rank r) {
    switch (r) {
    case RANK_1:
    case RANK_2:
    case RANK_3:
    case RANK_4:
    case RANK_5:
    case RANK_6:
    case RANK_7:
    case RANK_8:
        return true;
    default:
        return false;
    }
}

bool sq_is_valid_file(enum file f) {
    switch (f) {
    case FILE_A:
    case FILE_B:
    case FILE_C:
    case FILE_D:
    case FILE_E:
    case FILE_F:
    case FILE_G:
    case FILE_H:
        return true;
    default:
        return false;
    }
}

/**
 * @brief       Returns the square given the associated rank and file
 * @param rank  The rank
 * @param file  The file
 * @return      The square that is the given rank and file
 */
enum square sq_gen_from_rank_file(enum rank rank, enum file file) {
    assert(validate_rank(rank));
    assert(validate_file(file));

    return (enum square)((rank << 3) + file);
}

/**
 * @brief       Returns the square in the rank/file format (eg, "a1")
 * @param sq    The square
 * @return      The char array
 */
char *print_square(enum square sq) {
    assert(validate_square(sq));

    enum file f = sq_get_file(sq);
    enum rank r = sq_get_rank(sq);

    sprintf(square_text, "%c%c", ('a' + (int)f), ('1' + (int)r));

    return square_text;
}

/**
 * @brief       Validates the given square is within normal range
 * @param sq    The square
 * @return      true if valid, false otherwise
 */
bool validate_square(enum square sq) {
    return (sq >= a1) && (sq <= h8);
}

/**
 * @brief       Validates the given rank is within normal range
 * @param rank  The rank
 * @return      true if valid, false otherwise
 */
bool validate_rank(enum rank rank) {
    return sq_is_valid_rank(rank);
}

/**
 * @brief       Validates the given file is within normal range
 * @param file  The file
 * @return      true if valid, false otherwise
 */
bool validate_file(enum file file) {
    return sq_is_valid_file(file);
}

/**
 * @brief               Validates the given file is a valid en passent square
 * @param sq_to_check   The file
 * @return              true if valid, false otherwise
 */

bool validate_en_pass_sq(enum square sq_to_check) {
    validate_square(sq_to_check);
    enum rank r = sq_get_rank(sq_to_check);

    switch (r) {
    case RANK_3:
    case RANK_6:
        return true;
    case RANK_1:
    case RANK_2:
    case RANK_4:
    case RANK_5:
    case RANK_7:
    case RANK_8:
        return false;
    default:
        return false;
    }
}

/**
 * @brief Try convert he given rank and file to a square. Rank and File are 
 * validated (since they can be outside the range)
 * 
 * @param r     rank
 * @param f     file
 * @return      valid square or NO_SQUARE
 */
enum square sq_try_get_sq(enum rank r, enum file f) {
    if (sq_is_valid_rank(r) && sq_is_valid_file(f)) {
        return sq_gen_from_rank_file(r, f);
    }

    return (enum square)NO_SQUARE;
}
