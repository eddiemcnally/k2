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

#include "board.h"
#include "bitboard.h"
#include "move.h"
#include "piece.h"
#include "square.h"
#include "utils.h"
#include <assert.h>

/**
 * @brief       Represents the state of the board (squares, pieces)
 */
struct board {
    uint32_t struct_init_key;

    // a set bit represents an occupied square
    uint64_t bb_board;

    // a bitboard per colour, a set bit means that colour occupies that square
    uint64_t bb_colour[NUM_COLOURS];

    // total material value for each colour
    uint32_t material[NUM_COLOURS];

    // contains the piece on a given square
    struct piece pce_square[NUM_SQUARES];

    // bitboard for each piece
    uint64_t piece_bb[NUM_COLOURS][NUM_PIECE_ROLES];
};

static_assert((int)(sizeof(struct board)) == BOARD_SIZE_BYTES,
              "Board is copied, check for -fshort-enums compiler option is "
              "enabled for efficiency.");
static_assert(BOARD_SIZE_BYTES == 200, "Incorrect value for BOARD_SIZE_BYTES");

// used to check struct is populated when passed into public functions
static const uint32_t STRUCT_INIT_KEY = 0xdeadbeef;

static bool validate_struct_init(const struct board *brd);
static bool validate_square_empty(const struct board *brd,
                                  const enum square to_sq);
static bool validate_pce_on_sq(const struct board *brd, const struct piece pce,
                               enum square sq);
static void add_material(struct board *brd, struct piece pce);
static void remove_material(struct board *brd, struct piece pce);
static void init_struct(struct board *brd);
static void clear_bitboards(struct board *brd, const struct piece pce,
                            const enum square sq);
static void set_bitboards(struct board *brd, const struct piece pce,
                          const enum square sq);
static void move_bitboards(struct board *brd, const struct piece pce,
                           const enum square from_sq, const enum square to_sq);

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
    struct board *retval = (struct board *)malloc(sizeof(struct board));
    init_struct(retval);

    return retval;
}

/**
 * @brief       Clears and Deallocates the memory associated with a board
 *
 * @param brd   Pointer to the struct memory
 */
void brd_deallocate(struct board *brd) {
    assert(validate_struct_init(brd));

    memset(brd, 0, sizeof(struct board));
    free(brd);
}

/**
 * @brief       Gets a bitboard for the entire board
 *
 * @param brd The board
 * @return A bitboard with a bit set for each occupier square
 */
uint64_t brd_get_board_bb(const struct board *brd) {
    assert(validate_board(brd));
    return brd->bb_board;
}

/**
 * @brief        Tests if given square is set on board
 * @param   brd  the board
 * @param   sq   the square to test
 * @return  true if square occupied, false otherwise
 */
bool brd_is_sq_occupied(const struct board *brd, const enum square sq) {
    assert(validate_board(brd));
    assert(validate_square(sq));

    return bb_is_set(brd->bb_board, sq);
}

/**
 * @brief       Tries to return the piece on the given square
 * @param brd   the board
 * @param sq    the square
 * @param pce   the returned piece (if true)
 * @return true is piece found, false otherwise
 */
bool brd_try_get_piece_on_square(const struct board *brd, const enum square sq,
                                 struct piece *pce) {
    assert(validate_board(brd));
    assert(validate_square(sq));

    if (brd_is_sq_occupied(brd, sq)) {
        *pce = brd->pce_square[sq];
        return true;
    }
    return false;
}

/**
 * @brief       Add a piece to the board on the specified square
 *
 * @param brd   The board
 * @param pce   The piece to add
 * @param sq    The square
 */
void brd_add_piece(struct board *brd, const struct piece pce,
                   const enum square sq) {
    assert(validate_board(brd));
    assert(validate_square(sq));
    assert(validate_piece(pce));
    assert(validate_square_empty(brd, sq));

    set_bitboards(brd, pce, sq);
    add_material(brd, pce);
}

/**
 * @brief Returns the material value for the given side/colour 
 * 
 * @param brd           The board
 * @param side          The side 
 * @return uint32_t     The current material value
 */
uint32_t brd_get_material(const struct board *brd, const enum colour side) {
    assert(validate_board(brd));
    assert(validate_colour(side));

    uint8_t idx = pce_col_get_array_idx(side);
    return brd->material[idx];
}

/**
 * @brief       Removes a piece from the specified square
 *
 * @param brd   The board
 * @param pce   The piece
 * @param sq    The square
 */
void brd_remove_piece(struct board *brd, const struct piece pce,
                      const enum square sq) {

    assert(brd_is_sq_occupied(brd, sq) == true);
    assert(validate_board(brd));
    assert(validate_square(sq));
    assert(validate_piece(pce));
    assert(validate_pce_on_sq(brd, pce, sq));

    clear_bitboards(brd, pce, sq);
    remove_material(brd, pce);
}

/**
 * @brief       Move a piece from the "From" square to the "To" square
 *
 * @param brd   The board
 * @param pce   The piece to move
 * @param mv    The move
 */
void brd_move_piece(struct board *brd, const struct piece pce,
                    const enum square from_sq, const enum square to_sq) {

    assert(validate_board(brd));
    assert(validate_piece(pce));
    assert(validate_pce_on_sq(brd, pce, from_sq));
    assert(brd_is_sq_occupied(brd, to_sq) == false);
    assert(brd_is_sq_occupied(brd, from_sq));

    move_bitboards(brd, pce, from_sq, to_sq);
}

/**
 * @brief       Gets the bitboard representing all pieces of the given colour
 *
 * @param brd   The board
 * @param colour The colour
 *
 * @return A bitboard with a bit set for each piece of the given colour
 */
uint64_t brd_get_colour_bb(const struct board *brd, const enum colour colour) {
    assert(validate_colour(colour));
    assert(validate_board(brd));

    uint8_t offset = pce_col_get_array_idx(colour);
    return brd->bb_colour[offset];
}

/**
 * @brief       Returns a bitboard for the given piece, representing the location of all pieces of the given type.
 *
 * @param brd The board
 * @param pce The piece
 *
 * @return A bitboard for that piece
 */
uint64_t brd_get_piece_bb(const struct board *brd,
                          const enum piece_role pce_role,
                          const enum colour col) {
    assert(validate_piece_role(pce_role));
    assert(validate_board(brd));
    assert(validate_colour(col));

    const uint8_t pce_off = pce_get_array_idx(pce_role);
    const uint8_t col_off = pce_col_get_array_idx(col);

    return brd->piece_bb[col_off][pce_off];
}

/**
 * @brief       Validates that the internal state representation of the board is consistent
 *
 * @param brd The board to validate
 */
bool validate_board(const struct board *brd) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    enum square sq;

    if (brd->struct_init_key != STRUCT_INIT_KEY) {
        assert(false);
    }

    const uint8_t white_idx = pce_col_get_array_idx(WHITE);
    const uint8_t black_idx = pce_col_get_array_idx(BLACK);

    // conflate colour bitboards
    const uint64_t white_bb = brd->bb_colour[white_idx];
    const uint64_t black_bb = brd->bb_colour[black_idx];
    const uint64_t conflated_col_bb = white_bb | black_bb;

    // check various bitboards agree with the pieces on the squares
    for (sq = a1; sq <= h8; sq++) {
        const bool is_occupied = bb_is_set(brd->bb_board, sq);

        if (is_occupied) {
            assert(bb_is_set(conflated_col_bb, sq));
            assert(pce_are_equal(brd->pce_square[sq], pce_get_no_piece()) ==
                   false);
            assert(bb_is_set(brd->bb_board, sq));
            assert(bb_is_set(
                brd->bb_colour[pce_get_colour(brd->pce_square[sq])], sq));
        } else {
            assert(bb_is_clear(conflated_col_bb, sq));
            assert(pce_are_equal(brd->pce_square[sq], pce_get_no_piece()));
            assert(bb_is_clear(brd->bb_board, sq));
            assert(bb_is_clear(white_bb, sq));
            assert(bb_is_clear(black_bb, sq));
        }
    }

    // colour bitboards should AND to zero
    assert((brd->bb_colour[WHITE] & brd->bb_colour[BLACK]) == 0 &&
           "col bb not ANDing as zero");

    // can't be more bits set than max pieces on board
    const uint8_t num_bits_on_board =
        (uint8_t)__builtin_popcountll(brd->bb_board);
    assert(num_bits_on_board <= 32);

    // check colour bitboards and conflated board are the same
    uint8_t num_white_bits =
        (uint8_t)__builtin_popcountll(brd->bb_colour[WHITE]);
    uint8_t num_black_bits =
        (uint8_t)__builtin_popcountll(brd->bb_colour[BLACK]);
    assert(num_bits_on_board == (num_white_bits + num_black_bits));

    // conflate all piece bitboards, and verify same as board bitboard
    uint64_t conflated_pce_bb = 0;
    uint8_t total_bit_count = 0;
    for (int c = 0; c < NUM_COLOURS; c++) {
        for (int i = 0; i < NUM_PIECE_ROLES; i++) {
            conflated_pce_bb |= brd->piece_bb[c][i];
            total_bit_count +=
                (uint8_t)__builtin_popcountll(brd->piece_bb[c][i]);
        }
    }
    assert(conflated_pce_bb == brd->bb_board);
    assert(total_bit_count == num_bits_on_board);

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

bool brd_compare(const struct board *first, const struct board *second) {
    assert(validate_board(first));
    assert(validate_board(second));

    if (first->bb_board != second->bb_board) {
        return false;
    }

    for (int i = 0; i < NUM_COLOURS; i++) {
        if (first->bb_colour[i] != second->bb_colour[i]) {
            return false;
        }
        if (first->material[i] != second->material[i]) {
            return false;
        }
    }

    for (int i = 0; i < NUM_SQUARES; i++) {
        if (pce_are_equal(first->pce_square[i], second->pce_square[i]) ==
            false) {
            return false;
        }
    }

    for (int c = 0; c < NUM_COLOURS; c++) {
        for (int i = 0; i < NUM_PIECE_ROLES; i++) {
            if (first->piece_bb[c][i] != second->piece_bb[c][i]) {
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief Clones the current board
 * 
 * @param source    The source board to clone
 * @param dest      The target for the clone
 */
void brd_clone(const struct board *source, struct board *dest) {
    assert(validate_board(source));

    memcpy(dest, source, sizeof(struct board));
}

void brd_print(const struct board *brd) {

    printf("\nGame Board:\n\n");

    for (int r = RANK_8; r >= RANK_1; r--) {
        printf("%d  ", r + 1);
        for (int f = FILE_A; f <= FILE_H; f++) {
            const enum square sq =
                sq_gen_from_rank_file((enum rank)r, (enum file)f);
            struct piece pce;
            const bool found = brd_try_get_piece_on_square(brd, sq, &pce);
            if (found) {
                printf("%3c", pce_get_label(pce));
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
}

// ==================================================================
//
// private functions
//
// ==================================================================

static void init_struct(struct board *brd) {
    memset(brd, 0, sizeof(struct board));

    brd->bb_board = bb_get_empty();
    for (int i = 0; i < NUM_COLOURS; i++) {
        brd->bb_colour[i] = bb_get_empty();
        brd->material[i] = 0;
    }

    for (enum square sq = a1; sq <= h8; sq++) {
        brd->pce_square[sq] = pce_get_no_piece();
    }

    brd->struct_init_key = STRUCT_INIT_KEY;
}

static void add_material(struct board *brd, struct piece pce) {
    const enum piece_role pt = pce_get_piece_role(pce);
    const uint32_t material = pce_get_value(pt);
    const enum colour col = pce_get_colour(pce);
    const uint8_t offset = pce_col_get_array_idx(col);
    brd->material[offset] += material;
}

static void remove_material(struct board *brd, struct piece pce) {
    const enum piece_role pt = pce_get_piece_role(pce);
    const uint32_t material = pce_get_value(pt);
    const enum colour col = pce_get_colour(pce);
    const uint8_t offset = pce_col_get_array_idx(col);
    brd->material[offset] -= material;
}

static void clear_bitboards(struct board *brd, const struct piece pce,
                            const enum square sq) {
    const uint8_t pce_off = pce_get_array_idx(pce_get_piece_role(pce));
    const uint8_t col_off = pce_col_get_array_idx(pce_get_colour(pce));

    uint64_t *p_pce_bb = &brd->piece_bb[col_off][pce_off];
    uint64_t *p_brd_bb = &brd->bb_board;
    uint64_t *p_col_bb = &brd->bb_colour[col_off];

    bb_clear_square(p_pce_bb, sq);
    bb_clear_square(p_brd_bb, sq);
    bb_clear_square(p_col_bb, sq);
    brd->pce_square[sq] = pce_get_no_piece();
}

static void set_bitboards(struct board *brd, const struct piece pce,
                          const enum square sq) {
    const uint8_t pce_off = pce_get_array_idx(pce_get_piece_role(pce));
    const uint8_t col_off = pce_col_get_array_idx(pce_get_colour(pce));

    uint64_t *p_pce_bb = &brd->piece_bb[col_off][pce_off];
    uint64_t *p_brd_bb = &brd->bb_board;
    uint64_t *p_col_bb = &brd->bb_colour[col_off];

    bb_set_square(p_pce_bb, sq);
    bb_set_square(p_brd_bb, sq);
    bb_set_square(p_col_bb, sq);
    brd->pce_square[sq] = pce;
}

static void move_bitboards(struct board *brd, const struct piece pce,
                           const enum square from_sq, const enum square to_sq) {
    const uint8_t pce_off = pce_get_array_idx(pce_get_piece_role(pce));
    const uint8_t col_off = pce_col_get_array_idx(pce_get_colour(pce));

    uint64_t *p_pce_bb = &brd->piece_bb[col_off][pce_off];
    uint64_t *p_brd_bb = &brd->bb_board;
    uint64_t *p_col_bb = &brd->bb_colour[col_off];

    bb_clear_square(p_pce_bb, from_sq);
    bb_clear_square(p_brd_bb, from_sq);
    bb_clear_square(p_col_bb, from_sq);
    brd->pce_square[from_sq] = pce_get_no_piece();

    bb_set_square(p_pce_bb, to_sq);
    bb_set_square(p_brd_bb, to_sq);
    bb_set_square(p_col_bb, to_sq);
    brd->pce_square[to_sq] = pce;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static bool validate_struct_init(const struct board *brd) {
    return brd->struct_init_key == STRUCT_INIT_KEY;
}

static bool validate_square_empty(const struct board *brd,
                                  const enum square sq) {
    uint64_t bb = brd_get_board_bb(brd);
    bool is_set = bb_is_set(bb, sq);
    return is_set == false;
}

static bool validate_pce_on_sq(const struct board *brd, const struct piece pce,
                               enum square sq) {
    struct piece pce_on_brd;
    bool found = brd_try_get_piece_on_square(brd, sq, &pce_on_brd);
    if (found == false) {
        return false;
    }

    return pce_are_equal(pce_on_brd, pce);
}
#pragma GCC diagnostic pop
