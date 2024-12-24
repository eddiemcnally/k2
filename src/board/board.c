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
/*! @addtogroup Board
 *
 * @ingroup Board
 * @{
 * @details This the board representation within the Engine.
 *
 */

#include "board.h"
#include "bitboard.h"
#include "move.h"
#include "piece.h"
#include "score.h"
#include "square.h"
#include "utils.h"
#include <assert.h>

struct colour_info {
    uint64_t colour_bb;
    uint64_t piece_bb[NUM_PIECE_ROLES];
    Score material;
    enum square king_sq;
};

/**
 * @brief struct for containing the board
 * 
 */
struct board {
    struct colour_info colour_info[NUM_COLOURS];

    // contains the piece on a given square
    enum piece pce_square[NUM_SQUARES];

    uint32_t init_flag;
};

static bool validate_square_empty(const struct board *brd, const enum square to_sq);
static bool validate_pce_on_sq(const struct board *brd, const enum piece pce, enum square sq);
static void init_struct(struct board *brd);

static const uint32_t INIT_KEY = 0xDEADBEEF;

// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief       Allocates and initialises a board struct
 * @return      ptr to struct board
 */
struct board *brd_allocate(void) {
    struct board *retval = (struct board *)calloc(1, sizeof(struct board));
    init_struct(retval);

    return retval;
}

/**
 * @brief De-allocated the board
 * 
 * @param brd The board
 */
void brd_deallocate(struct board *brd) {
    if (brd->init_flag != INIT_KEY) {
        print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "Board struct isn't initialised");
    }
    memset(brd, 0, sizeof(struct board));
    free(brd);
}

/**
 * @brief       Gets a bitboard for the entire board
 *
 * @param brd The board
 * @return A bitboard with a bit set for each occupier square
 */
uint64_t brd_get_board_bb(const struct board *const brd) {
    assert(validate_board(brd));

    return brd->colour_info[WHITE].colour_bb | brd->colour_info[BLACK].colour_bb;
}

/**
 * @brief        Tests if given square is set on board
 * @param   brd  the board
 * @param   sq   the square to test
 * @return  true if square occupied, false otherwise
 */
bool brd_is_sq_occupied(const struct board *const brd, const enum square sq) {
    assert(validate_board(brd));
    assert(validate_square(sq));

    return brd->pce_square[sq] != NO_PIECE;
}

bool brd_try_get_piece_on_square(const struct board *const brd, const enum square sq, enum piece *piece) {
    assert(validate_board(brd));
    assert(validate_square(sq));

    *piece = brd->pce_square[sq];
    return *piece != NO_PIECE;
}

/**
 * @brief       Add a piece to the board on the specified square
 *
 * @param brd   The board
 * @param pce   The piece to add
 * @param sq    The square
 */
void brd_add_piece(struct board *const brd, const enum piece pce, const enum square sq) {

    assert(validate_square(sq));
    assert(validate_square_empty(brd, sq));
    assert(validate_piece(pce));
    assert(validate_board(brd));

    const enum colour colour = pce_get_colour(pce);
    const enum piece_role role = pce_get_role(pce);

    // update colour-related info
    const uint64_t sq_bb = SQUARE_AS_BITBOARD(sq);
    brd->colour_info[colour].colour_bb ^= sq_bb;
    brd->colour_info[colour].piece_bb[role] ^= sq_bb;

    const Score material = pce_get_value(pce);
    brd->colour_info[colour].material += material;

    if (role == KING) {
        brd->colour_info[colour].king_sq = sq;
    }

    // update generic board info
    brd->pce_square[sq] = pce;
}

/**
 * @brief Returns the material value for the given side/colour 
 * 
 * @param brd           The board
 * @return material     The current material value
 */
struct material brd_get_material(const struct board *const brd) {
    assert(validate_board(brd));

    struct material m = {.white = brd->colour_info[WHITE].material, .black = brd->colour_info[BLACK].material};
    return m;
}

void brd_remove_piece(struct board *const brd, const enum piece pce, const enum square sq) {

    assert(brd_is_sq_occupied(brd, sq) == true);
    assert(validate_board(brd));
    assert(validate_square(sq));
    assert(validate_pce_on_sq(brd, pce, sq));
    assert(validate_piece(pce));

    const enum colour colour = pce_get_colour(pce);
    const enum piece_role role = pce_get_role(pce);

    const uint64_t sq_bb = SQUARE_AS_BITBOARD(sq);
    brd->colour_info[colour].colour_bb ^= sq_bb;
    brd->colour_info[colour].piece_bb[role] ^= sq_bb;

    brd->pce_square[sq] = NO_PIECE;

    const Score material = pce_get_value(pce);
    brd->colour_info[colour].material -= material;
}

void brd_remove_from_square(struct board *const brd, const enum square sq) {
    assert(brd_is_sq_occupied(brd, sq) == true);

    enum piece piece;
    bool found = brd_try_get_piece_on_square(brd, sq, &piece);
    if (!found) {
        print_stacktrace_and_exit(__FILE__, __LINE__, __FUNCTION__, "No piece found when trying to remove from board");
    }

    brd_remove_piece(brd, piece, sq);
}

void brd_move_piece(struct board *const brd, const enum piece pce, const enum square from_sq, const enum square to_sq) {

    assert(validate_board(brd));
    assert(validate_piece(pce));
    assert(validate_pce_on_sq(brd, pce, from_sq));
    assert(brd_is_sq_occupied(brd, to_sq) == false);
    assert(brd_is_sq_occupied(brd, from_sq));

    const enum colour colour = pce_get_colour(pce);
    const enum piece_role role = pce_get_role(pce);

    // set/clear to/from squares in various bitboards
    const uint64_t from_bb = SQUARE_AS_BITBOARD(from_sq);
    const uint64_t to_bb = SQUARE_AS_BITBOARD(to_sq);

    brd->colour_info[colour].colour_bb ^= from_bb;
    brd->colour_info[colour].colour_bb ^= to_bb;
    brd->colour_info[colour].piece_bb[role] ^= from_bb;
    brd->colour_info[colour].piece_bb[role] ^= to_bb;

    assert(brd->pce_square[from_sq] == pce);

    brd->pce_square[from_sq] = NO_PIECE;
    brd->pce_square[to_sq] = pce;

    if (role == KING) {
        brd->colour_info[colour].king_sq = to_sq;
    }
}

/**
 * @brief      Returns a bitboard representing all black pieces
 *
 * @param[in]  brd   The brd
 *
 * @return     A bitboard with a bit set for each square containing a black piece
 */
uint64_t brd_get_black_bb(const struct board *const brd) {
    assert(validate_board(brd));

    return brd->colour_info[BLACK].colour_bb;
}

uint64_t brd_get_colour_bb(const struct board *const brd, const enum colour colour) {
    return brd->colour_info[colour].colour_bb;
}

/**
 * @brief      Returns a bitboard representing all white pieces
 *
 * @param[in]  brd   The brd
 *
 * @return     A bitboard with a bit set for each square containing a white piece
 */
uint64_t brd_get_white_bb(const struct board *const brd) {
    assert(validate_board(brd));

    return brd->colour_info[WHITE].colour_bb;
}

bool brd_try_get_colour_on_sq(const struct board *const brd, const enum square sq, enum colour *colour) {
    enum piece piece;
    bool found = brd_try_get_piece_on_square(brd, sq, &piece);
    if (!found) {
        return false;
    }

    *colour = pce_get_colour(piece);
    return true;
}

uint64_t brd_get_piece_bb(const struct board *const brd, const enum piece pce) {
    assert(validate_board(brd));

    const enum colour colour = pce_get_colour(pce);
    const enum piece_role role = pce_get_role(pce);

    return brd->colour_info[colour].piece_bb[role];
}

enum square brd_get_king_square(const struct board *const brd, const enum colour colour) {
    return brd->colour_info[colour].king_sq;
}

uint64_t brd_get_bb_for_role_colour(const struct board *const brd, const enum piece_role role,
                                    const enum colour colour) {

    return brd->colour_info[colour].piece_bb[role];
}

/**
 * @brief Returns a bitboard representing the rook and queen positions for a given colour
 * 
 * @param brd       The board
 * @param colour    The side
 * @return uint64_t The rook and queen bitboard
 */
uint64_t brd_get_rook_queen_bb_for_colour(const struct board *const brd, const enum colour colour) {
    return brd->colour_info[colour].piece_bb[ROOK] | brd->colour_info[colour].piece_bb[QUEEN];
}

/**
 * @brief Returns a bitboard representing the bishop and queen positions for a given colour
 * 
 * @param brd       The board
 * @param colour    The side
 * @return uint64_t The bishop and queen bitboard
 */
uint64_t brd_get_bishop_queen_bb_for_colour(const struct board *const brd, const enum colour colour) {

    return brd->colour_info[colour].piece_bb[BISHOP] | brd->colour_info[colour].piece_bb[QUEEN];
}

/**
 * @brief       Validates that the internal state representation of the board is consistent
 *
 * @param brd The board to validate
 */
bool validate_board(const struct board *const brd) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

    enum square sq;

    // conflate colour bitboards
    const uint64_t conflated_col_bb = brd->colour_info[WHITE].colour_bb | brd->colour_info[BLACK].colour_bb;

    // check various bitboards agree with the pieces on the squares
    for (sq = a1; sq <= h8; sq++) {
        const bool is_occupied = bb_is_set(conflated_col_bb, sq);

        if (is_occupied) {
            assert(bb_is_set(conflated_col_bb, sq));
            assert(brd->pce_square[sq] != NO_PIECE);

            const enum piece pce = brd->pce_square[sq];
            const enum colour col = pce_get_colour(pce);

            assert(bb_is_set(brd->colour_info[col].colour_bb, sq));
        } else {
            assert(bb_is_clear(conflated_col_bb, sq));
            assert(brd->pce_square[sq] == NO_PIECE);
        }
    }

    // colour bitboards should AND to zero
    assert((brd->colour_info[WHITE].colour_bb & brd->colour_info[BLACK].colour_bb) == 0 && "col bb not ANDing as zero");

    // can't be more bits set than max pieces on board
    const uint8_t num_bits_on_board = (uint8_t)__builtin_popcountll(conflated_col_bb);
    assert(num_bits_on_board <= 32);

    // check colour bitboards and conflated board are the same
    uint8_t num_white_bits = (uint8_t)__builtin_popcountll(brd->colour_info[WHITE].colour_bb);
    uint8_t num_black_bits = (uint8_t)__builtin_popcountll(brd->colour_info[BLACK].colour_bb);
    assert(num_bits_on_board == (num_white_bits + num_black_bits));

    // TODO - check bits set correspond to pieces on squares

    assert(brd->init_flag == INIT_KEY);

#pragma GCC diagnostic pop
    return true;
}

/**
 * @brief               Compares 2 boards for equivalency
 *
 * @param first         The first board
 * @param second        The second board
 * @return              True if the boards are the same, false otherwise
 */

bool brd_compare(const struct board *const first, const struct board *const second) {
    assert(validate_board(first));
    assert(validate_board(second));

    // compare WHITE colour_info
    const struct colour_info *col_info_white = &first->colour_info[WHITE];
    const struct colour_info *col_info_white_other = &second->colour_info[WHITE];

    if (col_info_white->material != col_info_white_other->material) {
        return false;
    }
    if (col_info_white->colour_bb != col_info_white_other->colour_bb) {
        return false;
    }
    for (int i = 0; i < NUM_PIECE_ROLES; i++) {
        if (col_info_white->piece_bb[i] != col_info_white_other->piece_bb[i]) {
            return false;
        }
    }
    if (col_info_white->king_sq != col_info_white_other->king_sq) {
        return false;
    }

    // compare BLACK colour_info
    const struct colour_info *col_info_black = &first->colour_info[BLACK];
    const struct colour_info *col_info_black_other = &second->colour_info[BLACK];

    if (col_info_black->material != col_info_black_other->material) {
        return false;
    }
    if (col_info_black->colour_bb != col_info_black_other->colour_bb) {
        return false;
    }
    for (int i = 0; i < NUM_PIECE_ROLES; i++) {
        if (col_info_black->piece_bb[i] != col_info_black_other->piece_bb[i]) {
            return false;
        }
    }
    if (col_info_black->king_sq != col_info_black_other->king_sq) {
        return false;
    }

    for (int i = 0; i < NUM_SQUARES; i++) {
        if (first->pce_square[i] != second->pce_square[i]) {
            return false;
        }
    }

    if (first->init_flag != second->init_flag) {
        return false;
    }

    return true;
}

/**
 * @brief Prints a representation of the board
 * 
 * @param brd The board
 */
void brd_print(const struct board *const brd) {

    printf("\nGame Board:\n\n");

    for (int r = RANK_8; r >= RANK_1; r--) {
        printf("%d  ", r + 1);
        for (int f = FILE_A; f <= FILE_H; f++) {
            const enum square sq = sq_gen_from_rank_file((enum rank)r, (enum file)f);

            if (brd_is_sq_occupied(brd, sq)) {
                enum piece piece;
                brd_try_get_piece_on_square(brd, sq, &piece);
                printf("%3c", pce_get_label(piece));
            } else {
                printf("  .");
            }
        }
        printf("\n");
    }

    printf("\n   ");
    for (enum file f = FILE_A; f <= FILE_H; f++) {
        printf("%3c", 'a' + f);
    }
    printf("\n   ");
}

// ==================================================================
//
// private functions
//
// ==================================================================

static void init_struct(struct board *const brd) {
    memset(brd, 0, sizeof(struct board));

    for (enum square sq = a1; sq <= h8; sq++) {
        brd->pce_square[sq] = NO_PIECE;
    }

    brd->init_flag = INIT_KEY;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static bool validate_square_empty(const struct board *brd, const enum square sq) {
    uint64_t bb = brd_get_board_bb(brd);
    bool is_set = bb_is_set(bb, sq);
    return is_set == false;
}

static bool validate_pce_on_sq(const struct board *const brd, const enum piece pce, enum square sq) {

    if (brd_is_sq_occupied(brd, sq) == false) {
        print_stacktrace();
        return false;
    }

    enum piece piece;
    brd_try_get_piece_on_square(brd, sq, &piece);
    return piece == pce;
}
#pragma GCC diagnostic pop
