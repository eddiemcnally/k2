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
#include "square.h"
#include "utils.h"
#include <assert.h>

/**
 * @brief struct for containing the board
 * 
 */
struct board {
    // bitboard for each piece
    uint64_t piece_bitboards[NUM_PIECES];

    // a bitboard per colour, a set bit means that colour occupies that square
    uint64_t bb_colour[NUM_COLOURS];

    // a bit set for every occupied square
    uint64_t bb_board;

    // total material value for each colour
    int32_t material[NUM_COLOURS];

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
    struct board *retval = (struct board *)malloc(sizeof(struct board));
    init_struct(retval);

    return retval;
}

/**
 * @brief De-allocated the board
 * 
 * @param brd 
 */
void brd_deallocate(struct board *brd) {
    memset(brd, 0, sizeof(struct board));
    free(brd);
}

/**
 * @brief       Gets a bitboard for the entire board
 *
 * @param brd The board
 * @return A bitboard with a bit set for each occupier square
 */
inline uint64_t brd_get_board_bb(const struct board *brd) {
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

    return brd->pce_square[sq] != NO_PIECE;
}

/**
 * @brief Returns the piece on the given square.
 * 
 * @param brd The board
 * @param sq The square
 * @return enum piece The piece on the given square
 */
enum piece brd_get_piece_on_square(const struct board *brd, const enum square sq) {
    assert(validate_board(brd));
    assert(validate_square(sq));

    return brd->pce_square[sq];
}

/**
 * @brief       Add a piece to the board on the specified square
 *
 * @param brd   The board
 * @param pce   The piece to add
 * @param sq    The square
 */
void brd_add_piece(struct board *brd, const enum piece pce, const enum square sq) {
    assert(validate_board(brd));
    assert(validate_square(sq));
    assert(validate_piece(pce));
    assert(validate_square_empty(brd, sq));

    // set bitboards
    const uint8_t pce_off = PCE_GET_ARRAY_INDEX(pce);
    const enum colour col = pce_get_colour(pce);
    const uint8_t col_off = PCE_COL_GET_ARRAY_INDEX(col);

    bb_set_square(&brd->piece_bitboards[pce_off], sq);
    bb_set_square(&brd->bb_colour[col_off], sq);
    bb_set_square(&brd->bb_board, sq);
    brd->pce_square[sq] = pce;

    // add material
    const uint32_t material = pce_get_value(pce);

    brd->material[col_off] += material;
}

/**
 * @brief Returns the material value for the given side/colour 
 * 
 * @param brd           The board
 * @param side          The side 
 * @return int32_t      The current material value
 */
struct material brd_get_material(const struct board *brd) {
    assert(validate_board(brd));

    struct material m = {.white = brd->material[PCE_COL_ARRAY_OFFSET_WHITE],
                         .black = brd->material[PCE_COL_ARRAY_OFFSET_BLACK]};

    return m;
}

/**
 * @brief       Removes a piece from the specified square
 *
 * @param brd   The board
 * @param pce   The piece
 * @param sq    The square
 */
void brd_remove_piece(struct board *brd, const enum piece pce, const enum square sq) {

    assert(brd_is_sq_occupied(brd, sq) == true);
    assert(validate_board(brd));
    assert(validate_square(sq));
    assert(validate_piece(pce));
    assert(validate_pce_on_sq(brd, pce, sq));

    const uint8_t pce_off = PCE_GET_ARRAY_INDEX(pce);
    const enum colour col = pce_get_colour(pce);
    const uint8_t col_off = PCE_COL_GET_ARRAY_INDEX(col);

    bb_clear_square(&brd->piece_bitboards[pce_off], sq);
    bb_clear_square(&brd->bb_colour[col_off], sq);
    bb_clear_square(&brd->bb_board, sq);
    brd->pce_square[sq] = NO_PIECE;

    const uint32_t material = pce_get_value(pce);

    brd->material[col_off] -= material;
    REQUIRE(brd->material[col_off] > 0, "Material can't be <= zero");
}

/**
 * @brief       Removes a piece from the specified square
 *
 * @param brd   The board
 * @param sq    The square
 */
void brd_remove_from_square(struct board *brd, const enum square sq) {

    assert(brd_is_sq_occupied(brd, sq) == true);
    assert(validate_board(brd));
    assert(validate_square(sq));

    const enum piece pce = brd_get_piece_on_square(brd, sq);
    brd_remove_piece(brd, pce, sq);
}

/**
 * @brief       Move a piece from the "From" square to the "To" square
 *
 * @param brd   The board
 * @param pce   The piece to move
 * @param mv    The move
 */
void brd_move_piece(struct board *brd, const enum piece pce, const enum square from_sq, const enum square to_sq) {

    assert(validate_board(brd));
    assert(validate_piece(pce));
    assert(validate_pce_on_sq(brd, pce, from_sq));
    assert(brd_is_sq_occupied(brd, to_sq) == false);
    assert(brd_is_sq_occupied(brd, from_sq));

    const uint8_t pce_off = PCE_GET_ARRAY_INDEX(pce);
    const enum colour col = pce_get_colour(pce);
    const uint8_t col_off = PCE_COL_GET_ARRAY_INDEX(col);

    // clear from_sq bits
    bb_clear_square(&brd->piece_bitboards[pce_off], from_sq);
    bb_clear_square(&brd->bb_colour[col_off], from_sq);
    bb_clear_square(&brd->bb_board, from_sq);

    // set to_sq bits
    bb_set_square(&brd->piece_bitboards[pce_off], to_sq);
    bb_set_square(&brd->bb_colour[col_off], to_sq);
    bb_set_square(&brd->bb_board, to_sq);

    brd->pce_square[from_sq] = NO_PIECE;
    brd->pce_square[to_sq] = pce;
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

    uint8_t offset = PCE_COL_GET_ARRAY_INDEX(colour);
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
uint64_t brd_get_piece_bb(const struct board *brd, const enum piece pce) {
    assert(validate_board(brd));
    assert(validate_piece(pce));

    const uint8_t offset = PCE_GET_ARRAY_INDEX(pce);
    return brd->piece_bitboards[offset];
}

/**
 * @brief   Returns the WHITE king square
 * 
 * @param brd The board 
 * @return enum square The white king square
 */
enum square brd_get_white_king_square(const struct board *brd) {
    const uint64_t king_bb = brd->piece_bitboards[PCE_ARRAY_OFFSET_WHITE_KING];
    return bb_pop_1st_bit(king_bb);
}

/**
 * @brief   Returns the BLACK king square
 * 
 * @param brd The board 
 * @return enum square The black king square
 */
enum square brd_get_black_king_square(const struct board *brd) {
    const uint64_t king_bb = brd->piece_bitboards[PCE_ARRAY_OFFSET_BLACK_KING];
    return bb_pop_1st_bit(king_bb);
}

/**
 * @brief Returns a bitboard representing the WHITE rook and queen positions
 * 
 * @param brd 
 * @return uint64_t The rook and queen bitboard
 */
uint64_t brd_get_white_rook_queen_bb(const struct board *brd) {
    return brd->piece_bitboards[PCE_ARRAY_OFFSET_WHITE_ROOK] | brd->piece_bitboards[PCE_ARRAY_OFFSET_WHITE_QUEEN];
}

/**
 * @brief Returns a bitboard representing the BLACK rook and queen positions
 * 
 * @param brd 
 * @return uint64_t The rook and queen bitboard
 */
uint64_t brd_get_black_rook_queen_bb(const struct board *brd) {
    return brd->piece_bitboards[PCE_ARRAY_OFFSET_BLACK_ROOK] | brd->piece_bitboards[PCE_ARRAY_OFFSET_BLACK_QUEEN];
}

/**
 * @brief Returns a bitboard representing the WHITE bishop and queen positions
 * 
 * @param brd 
 * @return uint64_t The bishop and queen bitboard
 */
uint64_t brd_get_white_bishop_queen_bb(const struct board *brd) {
    return brd->piece_bitboards[PCE_ARRAY_OFFSET_WHITE_BISHOP] | brd->piece_bitboards[PCE_ARRAY_OFFSET_WHITE_QUEEN];
}

/**
 * @brief Returns a bitboard representing the BLACK bishop and queen positions
 * 
 * @param brd 
 * @return uint64_t The bishop and queen bitboard
 */
uint64_t brd_get_black_bishop_queen_bb(const struct board *brd) {
    return brd->piece_bitboards[PCE_ARRAY_OFFSET_BLACK_BISHOP] | brd->piece_bitboards[PCE_ARRAY_OFFSET_BLACK_QUEEN];
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

    // conflate colour bitboards
    const uint64_t conflated_col_bb =
        brd->bb_colour[PCE_COL_ARRAY_OFFSET_WHITE] | brd->bb_colour[PCE_COL_ARRAY_OFFSET_BLACK];

    // check various bitboards agree with the pieces on the squares
    for (sq = a1; sq <= h8; sq++) {
        const bool is_occupied = bb_is_set(conflated_col_bb, sq);

        if (is_occupied) {
            assert(bb_is_set(conflated_col_bb, sq));
            assert(brd->pce_square[sq] != NO_PIECE);
            assert(bb_is_set(brd->bb_colour[pce_get_colour(brd->pce_square[sq])], sq));
        } else {
            assert(bb_is_clear(conflated_col_bb, sq));
            assert(brd->pce_square[sq] == NO_PIECE);
        }
    }

    // colour bitboards should AND to zero
    assert((brd->bb_colour[WHITE] & brd->bb_colour[BLACK]) == 0 && "col bb not ANDing as zero");

    // can't be more bits set than max pieces on board
    const uint8_t num_bits_on_board = (uint8_t)__builtin_popcountll(conflated_col_bb);
    assert(num_bits_on_board <= 32);

    // check colour bitboards and conflated board are the same
    uint8_t num_white_bits = (uint8_t)__builtin_popcountll(brd->bb_colour[WHITE]);
    uint8_t num_black_bits = (uint8_t)__builtin_popcountll(brd->bb_colour[BLACK]);
    assert(num_bits_on_board == (num_white_bits + num_black_bits));

    // conflate all piece bitboards, and verify same as board bitboard
    uint64_t conflated_pce_bb = 0;
    uint8_t total_bit_count = 0;

    enum piece pce_array[NUM_PIECES];
    pce_get_all_pieces(pce_array);
    for (int i = 0; i < NUM_PIECES; i++) {
        enum piece p = pce_array[i];

        const uint8_t offset = PCE_GET_ARRAY_INDEX(p);
        conflated_pce_bb |= brd->piece_bitboards[offset];
        total_bit_count += (uint8_t)__builtin_popcountll(brd->piece_bitboards[offset]);
    }

    assert(total_bit_count == num_bits_on_board);

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

bool brd_compare(const struct board *first, const struct board *second) {
    assert(validate_board(first));
    assert(validate_board(second));

    for (int i = 0; i < NUM_COLOURS; i++) {
        if (first->bb_colour[i] != second->bb_colour[i]) {
            return false;
        }
        if (first->material[i] != second->material[i]) {
            return false;
        }
    }

    for (int i = 0; i < NUM_SQUARES; i++) {
        if (first->pce_square[i] != second->pce_square[i]) {
            return false;
        }
    }

    enum piece pce_array[NUM_PIECES];
    pce_get_all_pieces(pce_array);
    for (int i = 0; i < NUM_PIECES; i++) {
        enum piece p = pce_array[i];
        const uint8_t offset = PCE_GET_ARRAY_INDEX(p);

        if (first->piece_bitboards[offset] != second->piece_bitboards[offset]) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Prints a representation of the board
 * 
 * @param brd 
 */
void brd_print(const struct board *brd) {

    printf("\nGame Board:\n\n");

    for (int r = RANK_8; r >= RANK_1; r--) {
        printf("%d  ", r + 1);
        for (int f = FILE_A; f <= FILE_H; f++) {
            const enum square sq = sq_gen_from_rank_file((enum rank)r, (enum file)f);

            if (brd_is_sq_occupied(brd, sq)) {
                enum piece pce = brd_get_piece_on_square(brd, sq);
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

    for (int i = 0; i < NUM_COLOURS; i++) {
        brd->bb_colour[i] = bb_get_empty();
        brd->material[i] = 0;
    }

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

static bool validate_pce_on_sq(const struct board *brd, const enum piece pce, enum square sq) {

    if (brd_is_sq_occupied(brd, sq) == false) {
        return false;
    }

    enum piece pce_on_brd = brd_get_piece_on_square(brd, sq);
    return pce_on_brd == pce;
}
#pragma GCC diagnostic pop
