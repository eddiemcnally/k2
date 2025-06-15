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

enum move_flag_bits { MV_FLG_BIT_PROMOTE = 0x80, MV_FLG_BIT_CAPTURE = 0x40 };

static const char *move_details(const struct move mv);

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
enum move_type move_get_move_type(struct move mv) {
    return mv.move_type;
}

/**
 * @brief           Encodes a quiet move using the given to and from squares
 *
 * @param from_sq   The from square
 * @param to_sq     The to square
 * @return The encoded move
 */
struct move move_encode_quiet(enum square from_sq, enum square to_sq) {

    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_QUIET};
    return mv;
}

/**
 * @brief Encodes a Knight promotion move
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_knight(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_PROMOTE_KNIGHT};
    return mv;
}

/**
 * @brief Encodes a Knight promotion move with capture
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_knight_with_capture(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_PROMOTE_KNIGHT_CAPTURE};
    return mv;
}

/**
 * @brief Encodes a Bishop promotion move
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_bishop(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_PROMOTE_BISHOP};
    return mv;
}

/**
 * @brief Encodes a Bishop promotion move with capture
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return move     Encoded move
 */
struct move move_encode_promote_bishop_with_capture(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_PROMOTE_BISHOP_CAPTURE};
    return mv;
}

/**
 * @brief Encodes a Rook promotion move
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return move     Encoded Move
 */
struct move move_encode_promote_rook(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_PROMOTE_ROOK};
    return mv;
}

/**
 * @brief Encodes a Rook promotion move with capture
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return move     Encoded move
 */
struct move move_encode_promote_rook_with_capture(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_PROMOTE_ROOK_CAPTURE};
    return mv;
}

/**
 * @brief Encodes a Queen promotion move
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_queen(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_PROMOTE_QUEEN};
    return mv;
}

/**
 * @brief Encodes a Queen promotion move with capture
 * 
 * @param from_sq   From square
 * @param to_sq     To square
 * @return uint64_tEncoded move
 */
struct move move_encode_promote_queen_with_capture(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_PROMOTE_QUEEN_CAPTURE};
    return mv;
}

/**
 * @brief       Encodes a capture move using the given squares
 *
 * @param from_sq The from square
 * @param to_sq The to square
 * @return The encoded move
 */
struct move move_encode_capture(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_CAPTURE};
    return mv;
}

/**
 * @brief       Encodes a White King-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_kingside_white(void) {
    struct move mv = {.from_sq = e1, .to_sq = g1, .move_type = MV_TYPE_KING_CASTLE};
    return mv;
}

/**
 * @brief       Encodes a Black King-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_kingside_black(void) {
    struct move mv = {.from_sq = e8, .to_sq = g8, .move_type = MV_TYPE_KING_CASTLE};
    return mv;
}

/**
 * @brief       Encodes a White Queen-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_queenside_white(void) {
    struct move mv = {.from_sq = e1, .to_sq = c1, .move_type = MV_TYPE_QUEEN_CASTLE};
    return mv;
}

/**
 * @brief       Encodes a Black Queen-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_queenside_black(void) {
    struct move mv = {.from_sq = e8, .to_sq = c8, .move_type = MV_TYPE_QUEEN_CASTLE};
    return mv;
}

/**
 * @brief       Encodes a double first move for a pawn
 * @param       from_sq the from square
 * @param       to_sq the to square
 * @return      The encoded move
 */
struct move move_encode_pawn_double_first(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_DOUBLE_PAWN};
    return mv;
}

/**
 * @brief       Encodes an En Passant move using the given squares
 *
 * @param from_sq The from square
 * @param to_sq The to square
 * @return The encoded move
 */
struct move move_encode_enpassant(enum square from_sq, enum square to_sq) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));

    struct move mv = {.from_sq = from_sq, .to_sq = to_sq, .move_type = MV_TYPE_EN_PASS};
    return mv;
}

/**
 * @brief       Extracts the from square from the given move
 *
 * @param mv The move
 * @return The from square
 */
enum square move_decode_from_sq(struct move mv) {
    return mv.from_sq;
}

/**
 * @brief       Extracts the to square from the given move
 *
 * @param mv The move
 * @return The to square
 */
enum square move_decode_to_sq(struct move mv) {
    return mv.to_sq;
}

/**
 * @brief       Tests the given uint16_t, returns true if Quiet, false otherwise
 *
 * @param mv    The move to test
 * @return      true if quiet, false otherwise
 */
bool move_is_quiet(const struct move mv) {
    return mv.move_type == MV_TYPE_QUIET;
}

/**
 * @brief       Tests the given uint16_t, returns true if Double pawn move, false otherwise
 *
 * @param mv    The move to test
 * @return      true if double pawn move, false otherwise
 */
bool move_is_double_pawn(struct move mv) {
    return mv.move_type == MV_TYPE_DOUBLE_PAWN;
}

/**
 * @brief       Tests the given uint16_t, returns true if King Castle move, false otherwise
 *
 * @param mv    The move to test
 * @return      true if king-side castle move, false otherwise
 */
bool move_is_king_castle(struct move mv) {
    return mv.move_type == MV_TYPE_KING_CASTLE;
}

/**
 * @brief       Tests the given uint16_t, returns true if Queen Castle move, false otherwise
 *
 * @param mv    The move to test
 * @return      true if queen-side castle move, false otherwise
 */
bool move_is_queen_castle(struct move mv) {
    return mv.move_type == MV_TYPE_QUEEN_CASTLE;
}

/**
 * @brief      Returns the move type
 *
 * @param[in]  mv    The move
 *
 * @return     The move type enum{ description_of_the_return_value }
 */
enum move_type move_get_type(struct move mv) {
    return mv.move_type;
}

/**
 * @brief       Tests the given move, returns true if a Capture move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if Capture, false otherwise
 */
bool move_is_capture(struct move mv) {
    assert(validate_move(mv));

    return (mv.move_type & MV_FLG_BIT_CAPTURE) != 0;
}

/**
 * @brief       Tests the given move, returns true if a Promotion move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if a promotion, false otherwise
 */
bool move_is_promotion(struct move mv) {
    assert(validate_move(mv));

    return (mv.move_type & MV_FLG_BIT_PROMOTE) != 0;
}

/**
 * @brief       Tests the given move, returns true if an En Passant move,
 * false otherwise
 *
 * @param mv The move to test
 * @return true if an En Passant move, false otherwise
 */
bool move_is_en_passant(struct move mv) {
    assert(validate_move(mv));

    return mv.move_type == MV_TYPE_EN_PASS;
}

/**
 * @brief       Tests the given move, returns true if a Castle move,
 * false otherwise
 *
 * @param mv The move to test
 * @return true if a Castle move, false otherwise
 */
bool move_is_castle(struct move mv) {
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
bool move_compare(struct move mv1, struct move mv2) {
    assert(validate_move(mv1));
    assert(validate_move(mv2));

    return mv1.from_sq == mv2.from_sq && mv1.to_sq == mv2.to_sq && mv1.move_type == mv2.move_type;
}

/**
 * @brief       Prints a move
 *
 * @param mv The move print
 * @return The move in test
 */
char *move_print(struct move mv) {
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

const char *move_details(struct move mv) {

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

bool validate_move(struct move mv) {

    const bool from_ok = validate_square(mv.from_sq);
    const bool to_ok = validate_square(mv.to_sq);

    bool move_type_ok = false;
    switch (mv.move_type) {
    case MV_TYPE_QUIET:
    case MV_TYPE_DOUBLE_PAWN:
    case MV_TYPE_KING_CASTLE:
    case MV_TYPE_QUEEN_CASTLE:
    case MV_TYPE_CAPTURE:
    case MV_TYPE_EN_PASS:
    case MV_TYPE_PROMOTE_KNIGHT:
    case MV_TYPE_PROMOTE_BISHOP:
    case MV_TYPE_PROMOTE_ROOK:
    case MV_TYPE_PROMOTE_QUEEN:
    case MV_TYPE_PROMOTE_KNIGHT_CAPTURE:
    case MV_TYPE_PROMOTE_BISHOP_CAPTURE:
    case MV_TYPE_PROMOTE_ROOK_CAPTURE:
    case MV_TYPE_PROMOTE_QUEEN_CAPTURE:
        move_type_ok = true;
        break;
    default:
        move_type_ok = false;
        break;
    }

    return from_ok && to_ok && move_type_ok;
}
