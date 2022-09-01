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
    // bitboard for each piece and colour
    uint64_t bb_piece[NUM_PIECES];

    // a bitboard per colour, a set bit means that colour occupies that square
    uint64_t bb_colour[NUM_COLOURS];

    // total material value for each colour
    int32_t material[NUM_COLOURS];

    // contains the piece on a given square
    enum piece pce_square[NUM_SQUARES];

    // king squares
    enum square king_squares[NUM_COLOURS];

    uint32_t init_flag;
};

// clang-format off
#define PCE_OFFSET_WHITE_ROOK   (PIECE_AS_ARRAY_OFFSET(WHITE_ROOK))
#define PCE_OFFSET_WHITE_BISHOP (PIECE_AS_ARRAY_OFFSET(WHITE_BISHOP))
#define PCE_OFFSET_WHITE_QUEEN  (PIECE_AS_ARRAY_OFFSET(WHITE_QUEEN))

#define PCE_OFFSET_BLACK_ROOK   (PIECE_AS_ARRAY_OFFSET(BLACK_ROOK))
#define PCE_OFFSET_BLACK_BISHOP (PIECE_AS_ARRAY_OFFSET(BLACK_BISHOP))
#define PCE_OFFSET_BLACK_QUEEN  (PIECE_AS_ARRAY_OFFSET(BLACK_QUEEN))

// clang-format on

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
    REQUIRE(brd->init_flag == INIT_KEY, "Board struct isn't initialised");
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

    return brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_WHITE] | brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_BLACK];
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

enum piece brd_get_piece_on_square(const struct board *const brd, const enum square sq) {
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
void brd_add_piece(struct board *const brd, const enum piece pce, const enum square sq) {

    assert(validate_square(sq));
    assert(validate_square_empty(brd, sq));
    assert(validate_piece(pce));
    assert(validate_board(brd));

    const enum colour colour = pce_get_colour(pce);
    const uint8_t col_off = COLOUR_AS_ARRAY_OFFSET(colour);

    bb_set_square(&brd->bb_piece[PIECE_AS_ARRAY_OFFSET(pce)], sq);
    bb_set_square(&brd->bb_colour[col_off], sq);
    brd->pce_square[sq] = pce;

    // add material
    const uint32_t material = pce_get_value(pce);
    brd->material[col_off] += material;

    if (pce_get_role(pce) == KING) {
        brd->king_squares[col_off] = sq;
    }
}

/**
 * @brief Returns the material value for the given side/colour 
 * 
 * @param brd           The board
 * @return int32_t      The current material value
 */
struct material brd_get_material(const struct board *const brd) {
    assert(validate_board(brd));

    struct material m = {.white = brd->material[COLOUR_AS_ARRAY_OFFSET_WHITE],
                         .black = brd->material[COLOUR_AS_ARRAY_OFFSET_BLACK]};

    return m;
}

void brd_remove_piece(struct board *const brd, const enum piece pce, const enum square sq) {

    assert(brd_is_sq_occupied(brd, sq) == true);
    assert(validate_board(brd));
    assert(validate_square(sq));
    assert(validate_pce_on_sq(brd, pce, sq));
    assert(validate_piece(pce));

    const enum colour col = pce_get_colour(pce);
    const int col_off = COLOUR_AS_ARRAY_OFFSET(col);

    bb_clear_square(&brd->bb_piece[PIECE_AS_ARRAY_OFFSET(pce)], sq);
    bb_clear_square(&brd->bb_colour[col_off], sq);
    brd->pce_square[sq] = NO_PIECE;

    const uint32_t material = pce_get_value(pce);
    brd->material[col_off] -= material;
    assert(brd->material[col_off] > 0);

    if (pce_get_role(pce) == KING) {
        REQUIRE(false, "Attempted to remove King from board");
    }
}

void brd_remove_from_square(struct board *const brd, const enum square sq) {
    assert(brd_is_sq_occupied(brd, sq) == true);

    const enum piece pce = brd_get_piece_on_square(brd, sq);

    brd_remove_piece(brd, pce, sq);
}

void brd_move_piece(struct board *const brd, const enum piece pce, const enum square from_sq, const enum square to_sq) {

    assert(validate_board(brd));
    assert(validate_piece(pce));
    assert(validate_pce_on_sq(brd, pce, from_sq));
    assert(brd_is_sq_occupied(brd, to_sq) == false);
    assert(brd_is_sq_occupied(brd, from_sq));

    const enum colour colour = pce_get_colour(pce);
    const uint8_t col_off = COLOUR_AS_ARRAY_OFFSET(colour);

    // set/clear to/from squares in various bitboards
    bb_move_bit(&brd->bb_piece[PIECE_AS_ARRAY_OFFSET(pce)], from_sq, to_sq);
    bb_move_bit(&brd->bb_colour[col_off], from_sq, to_sq);

    assert(brd->pce_square[from_sq] == pce);

    brd->pce_square[from_sq] = NO_PIECE;
    brd->pce_square[to_sq] = pce;

    if (pce_get_role(pce) == KING) {
        brd->king_squares[col_off] = to_sq;
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
    return brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_BLACK];
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
    return brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_WHITE];
}

enum colour brd_get_colour_on_sq(const struct board *const brd, const enum square sq) {
    const uint64_t white_bb = brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_WHITE];
    if (bb_is_set(white_bb, sq)) {
        return WHITE;
    }

    const uint64_t black_bb = brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_BLACK];
    if (bb_is_set(black_bb, sq)) {
        return BLACK;
    }

    REQUIRE(false, "Invalid colour");
}

uint64_t brd_get_piece_bb(const struct board *const brd, const enum piece pce) {
    assert(validate_board(brd));
    return brd->bb_piece[PIECE_AS_ARRAY_OFFSET(pce)];
}

/**
 * @brief   Returns the WHITE king square
 * 
 * @param brd The board 
 * @return enum square The white king square
 */
enum square brd_get_white_king_square(const struct board *const brd) {
    return brd->king_squares[COLOUR_AS_ARRAY_OFFSET(WHITE)];
}

/**
 * @brief   Returns the BLACK king square
 * 
 * @param brd The board 
 * @return enum square The black king square
 */
enum square brd_get_black_king_square(const struct board *const brd) {
    return brd->king_squares[COLOUR_AS_ARRAY_OFFSET(BLACK)];
}

/**
 * @brief Returns a bitboard representing the WHITE rook and queen positions
 * 
 * @param brd The board
 * @return uint64_t The rook and queen bitboard
 */
uint64_t brd_get_white_rook_queen_bb(const struct board *const brd) {
    return brd->bb_piece[PCE_OFFSET_WHITE_ROOK] | brd->bb_piece[PCE_OFFSET_WHITE_QUEEN];
}

/**
 * @brief Returns a bitboard representing the BLACK rook and queen positions
 * 
 * @param brd The board
 * @return uint64_t The rook and queen bitboard
 */
uint64_t brd_get_black_rook_queen_bb(const struct board *const brd) {
    return brd->bb_piece[PCE_OFFSET_BLACK_ROOK] | brd->bb_piece[PCE_OFFSET_BLACK_QUEEN];
}

/**
 * @brief Returns a bitboard representing the WHITE bishop and queen positions
 * 
 * @param brd The board
 * @return uint64_t The bishop and queen bitboard
 */
uint64_t brd_get_white_bishop_queen_bb(const struct board *const brd) {
    return brd->bb_piece[PCE_OFFSET_WHITE_BISHOP] | brd->bb_piece[PCE_OFFSET_WHITE_QUEEN];
}

/**
 * @brief Returns a bitboard representing the BLACK bishop and queen positions
 * 
 * @param brd The board
 * @return uint64_t The bishop and queen bitboard
 */
uint64_t brd_get_black_bishop_queen_bb(const struct board *const brd) {
    return brd->bb_piece[PCE_OFFSET_BLACK_BISHOP] | brd->bb_piece[PCE_OFFSET_BLACK_QUEEN];
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
    const uint64_t conflated_col_bb =
        brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_WHITE] | brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_BLACK];

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
    assert((brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_WHITE] & brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_BLACK]) == 0 &&
           "col bb not ANDing as zero");

    // can't be more bits set than max pieces on board
    const uint8_t num_bits_on_board = (uint8_t)__builtin_popcountll(conflated_col_bb);
    assert(num_bits_on_board <= 32);

    // check colour bitboards and conflated board are the same
    uint8_t num_white_bits = (uint8_t)__builtin_popcountll(brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_WHITE]);
    uint8_t num_black_bits = (uint8_t)__builtin_popcountll(brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_BLACK]);
    assert(num_bits_on_board == (num_white_bits + num_black_bits));

    // conflate all piece bitboards, and verify same as board bitboard
    uint64_t conflated_pce_bb = 0;
    uint8_t total_bit_count = 0;

    for (int i = 0; i < NUM_PIECES; i++) {

        conflated_pce_bb |= brd->bb_piece[i];
        total_bit_count += (uint8_t)__builtin_popcountll(brd->bb_piece[i]);
    }

    assert(total_bit_count == num_bits_on_board);
    assert(conflated_pce_bb ==
           (brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_WHITE] | brd->bb_colour[COLOUR_AS_ARRAY_OFFSET_BLACK]));

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

    for (int i = 0; i < NUM_PIECES; i++) {
        if (first->bb_piece[i] != second->bb_piece[i]) {
            return false;
        }
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
                const enum piece pce = brd_get_piece_on_square(brd, sq);
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
    printf("\n   ");
}

// ==================================================================
//
// private functions
//
// ==================================================================

static void init_struct(struct board *const brd) {
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

static bool validate_pce_on_sq(const struct board *const brd, const enum piece pce, enum square sq) {

    if (brd_is_sq_occupied(brd, sq) == false) {
        print_stacktrace();
        return false;
    }

    const enum piece pce_on_brd = brd_get_piece_on_square(brd, sq);
    return pce_on_brd == pce;
}
#pragma GCC diagnostic pop
