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

#include "move.h"
#include "piece.h"
#include "square.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*! @addtogroup Move
 *
 * @ingroup Move
 * @{
 * @details Provides functions to encode and decode moves.
 *
 */

/**
 * bitmap for move
 * See https://www.chessprogramming.org/Encoding_Moves
 *
 * ---- ---- --11 1111      To Square
 * ---- 1111 11-- ----      From Square
 * 0000 ---- ---- ----      Quiet move
 * 0001 ---- ---- ----      Double Pawn push
 * 0010 ---- ---- ----      King Castle
 * 0011 ---- ---- ----      Queen Castle
 * 0100 ---- ---- ----      Capture
 * 0101 ---- ---- ----      En Passant Capture
 * 1000 ---- ---- ----      Promotion Knight
 * 1001 ---- ---- ----      Promotion Bishop
 * 1010 ---- ---- ----      Promotion Rook
 * 1011 ---- ---- ----      Promotion Queen
 * 1100 ---- ---- ----      Promotion Knight Capture
 * 1101 ---- ---- ----      Promotion Bishop Capture
 * 1110 ---- ---- ----      Promotion Rook Capture
 * 1111 ---- ---- ----      Promotion Queen Capture
 */

static const char *move_details(const struct move mv);
static struct move encode_move(const enum square from_sq, const enum square to_sq, const enum move_type move_type);

// clang-format off

// bit masks for move
#define MV_MASK_TO_SQ   ((uint16_t)(0x003F))
#define MV_MASK_FROM_SQ ((uint16_t)(0x0FC0))
#define MV_MASK_FLAGS   ((uint16_t)(0xF000))

enum move_bits_shifts { 
    MV_SHFT_TO_SQ   = 0, 
    MV_SHFT_FROM_SQ = 6
};

enum move_flag_bits {
    MV_FLG_BIT_PROMOTE = 0x8000,
    MV_FLG_BIT_CAPTURE = 0x4000
};

#define ENCODE_TO_SQ(to_sq)         ((uint16_t)(((uint16_t)to_sq << MV_SHFT_TO_SQ) & MV_MASK_TO_SQ))
#define ENCODE_FROM_SQ(from_sq)     ((uint16_t)(((uint16_t)from_sq << MV_SHFT_FROM_SQ) & MV_MASK_FROM_SQ))


#define ENCODE_KING_CASTLE_WHITE    ((uint16_t)(ENCODE_FROM_SQ(e1) | ENCODE_TO_SQ(g1) | MV_TYPE_KING_CASTLE))
#define ENCODE_KING_CASTLE_BLACK    ((uint16_t)(ENCODE_FROM_SQ(e8) | ENCODE_TO_SQ(g8) | MV_TYPE_KING_CASTLE))
#define ENCODE_QUEEN_CASTLE_WHITE   ((uint16_t)(ENCODE_FROM_SQ(e1) | ENCODE_TO_SQ(c1) | MV_TYPE_QUEEN_CASTLE))
#define ENCODE_QUEEN_CASTLE_BLACK   ((uint16_t)(ENCODE_FROM_SQ(e8) | ENCODE_TO_SQ(c8) | MV_TYPE_QUEEN_CASTLE))

static const struct move MOVE_KING_CASTLE_WHITE     = {.bitmap = ENCODE_KING_CASTLE_WHITE};
static const struct move MOVE_QUEEN_CASTLE_WHITE    = {.bitmap = ENCODE_QUEEN_CASTLE_WHITE};
static const struct move MOVE_KING_CASTLE_BLACK     = {.bitmap = ENCODE_KING_CASTLE_BLACK};
static const struct move MOVE_QUEEN_CASTLE_BLACK    = {.bitmap = ENCODE_QUEEN_CASTLE_BLACK};

// clang-format on

// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief Extracts the move type from the move
 * 
 * @param mv The move
 * @return enum move_type The move type 
 */
enum move_type move_get_move_type(const struct move mv) {
    return (enum move_type)(((uint16_t)mv.bitmap) & MV_MASK_FLAGS);
}

/**
 * @brief           Encodes a quiet move using the given to and from squares
 *
 * @param from_sq   The from square
 * @param to_sq     The to square
 * @return The encoded move
 */
struct move move_encode_quiet(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_QUIET);
}

/**
 * @brief Encodes a Knight promotion move
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_knight(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_PROMOTE_KNIGHT);
}

/**
 * @brief Encodes a Knight promotion move with capture
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_knight_with_capture(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_PROMOTE_KNIGHT_CAPTURE);
}

/**
 * @brief Encodes a Bishop promotion move
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_bishop(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_PROMOTE_BISHOP);
}

/**
 * @brief Encodes a Bishop promotion move with capture
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return move     Encoded move
 */
struct move move_encode_promote_bishop_with_capture(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_PROMOTE_BISHOP_CAPTURE);
}

/**
 * @brief Encodes a Rook promotion move
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return move     Encoded Move
 */
struct move move_encode_promote_rook(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_PROMOTE_ROOK);
}

/**
 * @brief Encodes a Rook promotion move with capture
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return move     Encoded move
 */
struct move move_encode_promote_rook_with_capture(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_PROMOTE_ROOK_CAPTURE);
}

/**
 * @brief Encodes a Queen promotion move
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_queen(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_PROMOTE_QUEEN);
}

/**
 * @brief Encodes a Queen promotion move with capture
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_queen_with_capture(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_PROMOTE_QUEEN_CAPTURE);
}

/**
 * @brief       Encodes a capture move using the given squares
 *
 * @param from_sq The from square
 * @param to_sq The to square
 * @return The encoded move
 */
struct move move_encode_capture(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));
    return encode_move(from_sq, to_sq, MV_TYPE_CAPTURE);
}

/**
 * @brief       Encodes a White King-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_kingside_white(void) {
    return MOVE_KING_CASTLE_WHITE;
}

/**
 * @brief       Encodes a Black King-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_kingside_black(void) {
    return MOVE_KING_CASTLE_BLACK;
}

/**
 * @brief       Encodes a White Queen-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_queenside_white(void) {
    return MOVE_QUEEN_CASTLE_WHITE;
}

/**
 * @brief       Encodes a Black Queen-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_queenside_black(void) {
    return MOVE_QUEEN_CASTLE_BLACK;
}

/**
 * @brief       Encodes a double first move for a pawn
 * @param       from_sq the from square
 * @param       to_sq the to square
 * @return      The encoded move
 */
struct move move_encode_pawn_double_first(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_DOUBLE_PAWN);
}

/**
 * @brief       Encodes an En Passant move using the given squares
 *
 * @param from_sq The from square
 * @param to_sq The to square
 * @return The encoded move
 */
struct move move_encode_enpassant(const enum square from_sq, const enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    return encode_move(from_sq, to_sq, MV_TYPE_EN_PASS);
}

/**
 * @brief       Extracts the from square from the given move
 *
 * @param mv The move
 * @return The from square
 */
enum square move_decode_from_sq(const struct move mv) {
    return (enum square)((mv.bitmap & MV_MASK_FROM_SQ) >> MV_SHFT_FROM_SQ);
}

/**
 * @brief       Extracts the to square from the given move
 *
 * @param mv The move
 * @return The to square
 */
enum square move_decode_to_sq(const struct move mv) {
    return (enum square)((mv.bitmap & MV_MASK_TO_SQ) >> MV_SHFT_TO_SQ);
}

/**
 * @brief       Tests the given uint16_t, returns true if Quiet, false otherwise
 *
 * @param mv    The move to test
 * @return      true if quiet, false otherwise
 */
bool move_is_quiet(const struct move mv) {
    return (mv.bitmap & MV_MASK_FLAGS) == MV_TYPE_QUIET;
}

/**
 * @brief       Tests the given uint16_t, returns true if Double pawn move, false otherwise
 *
 * @param mv    The move to test
 * @return      true if double pawn move, false otherwise
 */
bool move_is_double_pawn(const struct move mv) {
    return (mv.bitmap & MV_MASK_FLAGS) == MV_TYPE_DOUBLE_PAWN;
}

/**
 * @brief       Tests the given uint16_t, returns true if King Castle move, false otherwise
 *
 * @param mv    The move to test
 * @return      true if king-side castle move, false otherwise
 */
bool move_is_king_castle(const struct move mv) {
    return (mv.bitmap & MV_MASK_FLAGS) == MV_TYPE_KING_CASTLE;
}

/**
 * @brief       Tests the given uint16_t, returns true if Queen Castle move, false otherwise
 *
 * @param mv    The move to test
 * @return      true if queen-side castle move, false otherwise
 */
bool move_is_queen_castle(const struct move mv) {
    return (mv.bitmap & MV_MASK_FLAGS) == MV_TYPE_QUEEN_CASTLE;
}

/**
 * @brief      Returns the move type
 *
 * @param[in]  mv    The move
 *
 * @return     The move type enum{ description_of_the_return_value }
 */
enum move_type move_get_type(const struct move mv) {
    return (enum move_type)(mv.bitmap & MV_MASK_FLAGS);
}

/**
 * @brief       Tests the given move, returns true if a Capture move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if Capture, false otherwise
 */
bool move_is_capture(const struct move mv) {
    assert(validate_move(mv));

    return (mv.bitmap & MV_FLG_BIT_CAPTURE) != 0;
}

/**
 * @brief       Tests the given move, returns true if a Promotion move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if a promotion, false otherwise
 */
bool move_is_promotion(const struct move mv) {
    assert(validate_move(mv));

    return (mv.bitmap & MV_FLG_BIT_PROMOTE) != 0;
}

/**
 * @brief       Tests the given move, returns true if an En Passant move,
 * false otherwise
 *
 * @param mv The move to test
 * @return true if an En Passant move, false otherwise
 */
bool move_is_en_passant(const struct move mv) {
    assert(validate_move(mv));

    return (mv.bitmap & MV_MASK_FLAGS) == MV_TYPE_EN_PASS;
}

/**
 * @brief       Tests the given move, returns true if a Castle move,
 * false otherwise
 *
 * @param mv The move to test
 * @return true if a Castle move, false otherwise
 */
bool move_is_castle(const struct move mv) {
    assert(validate_move(mv));

    return move_is_king_castle(mv) || move_is_queen_castle(mv);
}

/**
 * @brief       Compares 2 moves for equality
 *
 * @param mv1 The 1st move
 * @param mv2 The 2nd move
 * @return true if moves are the same, false otherwise
 */
bool move_compare(const struct move mv1, const struct move mv2) {
    assert(validate_move(mv1));
    assert(validate_move(mv2));

    return mv1.bitmap == mv2.bitmap;
}

static struct move encode_move(const enum square from_sq, const enum square to_sq, const enum move_type move_type) {
    uint16_t bitflags = ENCODE_FROM_SQ(from_sq);
    bitflags |= ENCODE_TO_SQ(to_sq);
    bitflags |= (uint16_t)move_type;

    const struct move m = {.bitmap = bitflags};
    return m;
}

/**
 * @brief       Prints a move
 *
 * @param mv The move print
 * @return The move in test
 */
char *move_print(const struct move mv) {
    assert(validate_move(mv));

    static char move_string[50];

    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);

    const enum rank from_rank = sq_get_rank(from_sq);
    const enum file from_file = sq_get_file(from_sq);

    const enum rank to_rank = sq_get_rank(to_sq);
    const enum file to_file = sq_get_file(to_sq);

    sprintf(move_string, "%c%c%c%c : %s", ('a' + from_file), ('1' + from_rank), ('a' + to_file), ('1' + to_rank),
            move_details(mv));

    return move_string;
}

const char *move_details(const struct move mv) {

    const enum move_type mt = move_get_type(mv);

    switch (mt) {
    case MV_TYPE_QUIET:
        return "Quiet";
    case MV_TYPE_DOUBLE_PAWN:
        return "Double Pawn";
    case MV_TYPE_KING_CASTLE:
        return "King Castle";
    case MV_TYPE_QUEEN_CASTLE:
        return "Queen Castle";
    case MV_TYPE_CAPTURE:
        return "Capture";
    case MV_TYPE_EN_PASS:
        return "En Passant";
    case MV_TYPE_PROMOTE_KNIGHT:
        return "Promote; Knight";
    case MV_TYPE_PROMOTE_BISHOP:
        return "Promote; Bishop";
    case MV_TYPE_PROMOTE_ROOK:
        return "Promote; Rook";
    case MV_TYPE_PROMOTE_QUEEN:
        return "Promote; Queen";
    case MV_TYPE_PROMOTE_KNIGHT_CAPTURE:
        return "Promote; Capture; Knight";
    case MV_TYPE_PROMOTE_BISHOP_CAPTURE:
        return "Promote; Capture; Bishop";
    case MV_TYPE_PROMOTE_ROOK_CAPTURE:
        return "Promote; Capture; Rook";
    case MV_TYPE_PROMOTE_QUEEN_CAPTURE:
        return "Promote; Capture; Queen";
    default:
        return "No Details";
    }
}

bool validate_move(const struct move mv) {
    const enum square from = move_decode_from_sq(mv);
    const enum square to = move_decode_to_sq(mv);
    const bool from_ok = validate_square(from);
    const bool to_ok = validate_square(to);

    return from_ok && to_ok;
}
