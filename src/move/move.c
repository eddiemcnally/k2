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

enum move_bits_masks {
    MV_MASK_TO_SQ = 0x003F,
    MV_MASK_FROM_SQ = 0x0FC0,
    MV_MASK_FLAGS = 0xF000,
};

enum move_bits_shifts {
    MV_SHFT_TO_SQ = 0,
    MV_SHFT_FROM_SQ = 6,
};

#define TYPE_TO_FLAG(type) ((uint16_t)(type << 12))
#define FLAG_TO_TYPE(flag) ((enum move_type)(flag >> 12))

enum move_flags {
    MV_FLG_QUIET = TYPE_TO_FLAG(MV_TYPE_QUIET),
    MV_FLG_DOUBLE_PAWN = TYPE_TO_FLAG(MV_TYPE_DOUBLE_PAWN),
    MV_FLG_KING_CASTLE = TYPE_TO_FLAG(MV_TYPE_KING_CASTLE),
    MV_FLG_QUEEN_CASTLE = TYPE_TO_FLAG(MV_TYPE_QUEEN_CASTLE),
    MV_FLG_CAPTURE = TYPE_TO_FLAG(MV_TYPE_CAPTURE),
    MV_FLG_EN_PASS = TYPE_TO_FLAG(MV_TYPE_EN_PASS),
    MV_FLG_PROMOTE_KNIGHT = TYPE_TO_FLAG(MV_TYPE_PROMOTE_KNIGHT),
    MV_FLG_PROMOTE_BISHOP = TYPE_TO_FLAG(MV_TYPE_PROMOTE_BISHOP),
    MV_FLG_PROMOTE_ROOK = TYPE_TO_FLAG(MV_TYPE_PROMOTE_ROOK),
    MV_FLG_PROMOTE_QUEEN = TYPE_TO_FLAG(MV_TYPE_PROMOTE_QUEEN),
    MV_FLG_PROMOTE_KNIGHT_CAPTURE = TYPE_TO_FLAG(MV_TYPE_PROMOTE_KNIGHT_CAPTURE),
    MV_FLG_PROMOTE_BISHOP_CAPTURE = TYPE_TO_FLAG(MV_TYPE_PROMOTE_BISHOP_CAPTURE),
    MV_FLG_PROMOTE_ROOK_CAPTURE = TYPE_TO_FLAG(MV_TYPE_PROMOTE_ROOK_CAPTURE),
    MV_FLG_PROMOTE_QUEEN_CAPTURE = TYPE_TO_FLAG(MV_TYPE_PROMOTE_QUEEN_CAPTURE),
};

enum move_flag_bits {
    MV_FLG_BIT_PROMOTE = 0x8000,
    MV_FLG_BIT_CAPTURE = 0x4000,
};

static uint16_t set_flag(const uint16_t mv, const uint16_t flag);
static struct move encode_from_to(const enum square from_sq, const enum square to_sq);
static const char *move_details(const struct move mv);

#define ENCODE_TO_SQ(to_sq) ((uint16_t)((to_sq << MV_SHFT_TO_SQ) & MV_MASK_TO_SQ))
#define ENCODE_FROM_SQ(from_sq) ((uint16_t)((from_sq << MV_SHFT_FROM_SQ) & MV_MASK_FROM_SQ))

#define ENCODE_KING_CASTLE_WHITE (ENCODE_FROM_SQ(e1) | ENCODE_TO_SQ(g1) | MV_FLG_KING_CASTLE)
#define ENCODE_KING_CASTLE_BLACK (ENCODE_FROM_SQ(e8) | ENCODE_TO_SQ(g8) | MV_FLG_KING_CASTLE)
#define ENCODE_QUEEN_CASTLE_WHITE (ENCODE_FROM_SQ(e1) | ENCODE_TO_SQ(c1) | MV_FLG_QUEEN_CASTLE)
#define ENCODE_QUEEN_CASTLE_BLACK (ENCODE_FROM_SQ(e8) | ENCODE_TO_SQ(c8) | MV_FLG_QUEEN_CASTLE)

// ==================================================================
//
// public functions
//
// ==================================================================

enum move_type move_get_move_type(const struct move mv) {
    const uint16_t flag = ((uint16_t)mv.val) & MV_MASK_FLAGS;
    return FLAG_TO_TYPE(flag);
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

    return encode_from_to(from_sq, to_sq);
}

/**
 * @brief           Encodes a promotion move, including a possibly capture
 *
 * @param from_sq   The from square
 * @param to_sq     The to square
 * @param promoted_piece The target promotion piece
 * @param is_capture true if also a capture move, false otherwise
 * @return The encoded move
 */
struct move move_encode_promoted(const enum square from_sq, const enum square to_sq,
                                 const enum piece_role promoted_piece, const bool is_capture) {
    assert(validate_square(from_sq));
    assert(validate_square(to_sq));
    assert(validate_piece_role(promoted_piece));

    struct move mv = encode_from_to(from_sq, to_sq);

    enum move_flags flg;
    switch (promoted_piece) {
    case KNIGHT:
        flg = MV_FLG_PROMOTE_KNIGHT;
        break;
    case BISHOP:
        flg = MV_FLG_PROMOTE_BISHOP;
        break;
    case ROOK:
        flg = MV_FLG_PROMOTE_ROOK;
        break;
    case QUEEN:
        flg = MV_FLG_PROMOTE_QUEEN;
        break;
    default:
        REQUIRE(false, "Invalid promotion piece");
    }

    mv.val = set_flag(mv.val, (uint16_t)flg);
    if (is_capture) {
        mv.val = set_flag(mv.val, MV_FLG_BIT_CAPTURE);
    }

    return mv;
}

/**
 * @brief  Decodes the promotion piece from the move
 * 
 * @param mv The move
 * @param side The side
 * @return enum piece The decoded piece 
 */
enum piece move_decode_promotion_piece(const struct move mv, const enum colour side) {
    assert(validate_move(mv));
    assert(validate_colour(side));

    const uint16_t m = mv.val & MV_MASK_FLAGS;

    switch (m) {
    case MV_FLG_PROMOTE_KNIGHT_CAPTURE:
    case MV_FLG_PROMOTE_KNIGHT:
        return side == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
    case MV_FLG_PROMOTE_BISHOP_CAPTURE:
    case MV_FLG_PROMOTE_BISHOP:
        return side == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
    case MV_FLG_PROMOTE_QUEEN_CAPTURE:
    case MV_FLG_PROMOTE_QUEEN:
        return side == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
    case MV_FLG_PROMOTE_ROOK_CAPTURE:
    case MV_FLG_PROMOTE_ROOK:
        return side == WHITE ? WHITE_ROOK : BLACK_ROOK;
    default:
        REQUIRE(false, "Invalid promotion piece");
    }

    REQUIRE(false, "Unexpected code path decoding promotion piece");
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

    struct move mv = encode_from_to(from_sq, to_sq);
    mv.val = set_flag(mv.val, MV_FLG_CAPTURE);
    return mv;
}

/**
 * @brief       Encodes a King-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_kingside(const enum colour side) {
    struct move retval = {0};
    retval.val = side == WHITE ? ENCODE_KING_CASTLE_WHITE : ENCODE_KING_CASTLE_BLACK;
    return retval;
}

/**
 * @brief       Encodes a Queen-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_queenside(const enum colour side) {
    struct move retval = {0};
    retval.val = side == WHITE ? ENCODE_QUEEN_CASTLE_WHITE : ENCODE_QUEEN_CASTLE_BLACK;
    return retval;
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

    struct move mv = encode_from_to(from_sq, to_sq);
    mv.val = set_flag(mv.val, MV_FLG_DOUBLE_PAWN);
    return mv;
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

    struct move mv = encode_from_to(from_sq, to_sq);
    mv.val = set_flag(mv.val, MV_FLG_EN_PASS);
    return mv;
}

/**
 * @brief       Extracts the from square from the given move
 *
 * @param mv The move
 * @return The from square
 */
inline enum square move_decode_from_sq(const struct move mv) {
    return (enum square)((mv.val & MV_MASK_FROM_SQ) >> MV_SHFT_FROM_SQ);
}

/**
 * @brief       Extracts the to square from the given move
 *
 * @param mv The move
 * @return The to square
 */
inline enum square move_decode_to_sq(const struct move mv) {
    return (enum square)((mv.val & MV_MASK_TO_SQ) >> MV_SHFT_TO_SQ);
}

/**
 * @brief       Tests the given uint16_t, returns true if Quiet, false otherwise
 *
 * @param mv    The move to test
 * @return      true if quiet, false otherwise
 */
inline bool move_is_quiet(const struct move mv) {
    uint16_t m = ((uint16_t)mv.val) & MV_MASK_FLAGS;
    return m == MV_FLG_QUIET;
}

/**
 * @brief       Tests the given uint16_t, returns true if Double pawn move, false otherwise
 *
 * @param mv    The move to test
 * @return      true if double pawn move, false otherwise
 */
inline bool move_is_double_pawn(const struct move mv) {
    uint16_t m = ((uint16_t)mv.val) & MV_MASK_FLAGS;
    return m == MV_FLG_DOUBLE_PAWN;
}

/**
 * @brief       Tests the given uint16_t, returns true if King Castle move, false otherwise
 *
 * @param mv    The move to test
 * @return      true if king-side castle move, false otherwise
 */
inline bool move_is_king_castle(const struct move mv) {
    uint16_t m = ((uint16_t)mv.val) & MV_MASK_FLAGS;
    return m == MV_FLG_KING_CASTLE;
}

/**
 * @brief       Tests the given uint16_t, returns true if Queen Castle move, false otherwise
 *
 * @param mv    The move to test
 * @return      true if queen-side castle move, false otherwise
 */
inline bool move_is_queen_castle(const struct move mv) {
    uint16_t m = ((uint16_t)mv.val) & MV_MASK_FLAGS;
    return m == MV_FLG_QUEEN_CASTLE;
}

/**
 * @brief       Tests the given move, returns true if a Capture move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if Capture, false otherwise
 */
inline bool move_is_capture(const struct move mv) {
    assert(validate_move(mv));

    return (mv.val & MV_FLG_BIT_CAPTURE) != 0;
}

/**
 * @brief       Tests the given move, returns true if a Promotion move, false
 * otherwise
 *
 * @param mv The move to test
 * @return true if a promotion, false otherwise
 */
inline bool move_is_promotion(const struct move mv) {
    assert(validate_move(mv));

    return (mv.val & MV_FLG_BIT_PROMOTE) != 0;
}

/**
 * @brief       Tests the given move, returns true if an En Passant move,
 * false otherwise
 *
 * @param mv The move to test
 * @return true if an En Passant move, false otherwise
 */
inline bool move_is_en_passant(const struct move mv) {
    assert(validate_move(mv));

    uint16_t m = ((uint16_t)mv.val) & MV_MASK_FLAGS;
    return m == MV_FLG_EN_PASS;
}

/**
 * @brief       Tests the given move, returns true if a Castle move,
 * false otherwise
 *
 * @param mv The move to test
 * @return true if a Castle move, false otherwise
 */
inline bool move_is_castle(const struct move mv) {
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
    return mv1.val == mv2.val;
}

/**
 * @brief       Prints a move
 *
 * @param mv The move print
 * @return The move in test
 */
char *move_print(const struct move mv) {
    assert(validate_move(mv));

    static char move_string[6];

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

/*
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
const char *move_details(const struct move mv) {

    uint16_t details = (uint16_t)(mv.val & (uint16_t)MV_MASK_FLAGS);

    switch (details) {
    case MV_FLG_QUIET:
        return "Quiet";
    case MV_FLG_DOUBLE_PAWN:
        return "Double Pawn";
    case MV_FLG_KING_CASTLE:
        return "King Castle";
    case MV_FLG_QUEEN_CASTLE:
        return "Queen Castle";
    case MV_FLG_CAPTURE:
        return "Capture";
    case MV_FLG_EN_PASS:
        return "En Passant";
    case MV_FLG_PROMOTE_KNIGHT:
        return "Promote; Knight";
    case MV_FLG_PROMOTE_BISHOP:
        return "Promote; Bishop";
    case MV_FLG_PROMOTE_ROOK:
        return "Promote; Rook";
    case MV_FLG_PROMOTE_QUEEN:
        return "Promote; Queen";
    case MV_FLG_PROMOTE_KNIGHT_CAPTURE:
        return "Promote; Capture; Knight";
    case MV_FLG_PROMOTE_BISHOP_CAPTURE:
        return "Promote; Capture; Bishop";
    case MV_FLG_PROMOTE_ROOK_CAPTURE:
        return "Promote; Capture; Rook";
    case MV_FLG_PROMOTE_QUEEN_CAPTURE:
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

// ==================================================================
//
// private functions
//
// ==================================================================

__attribute__((always_inline)) struct move encode_from_to(const enum square from_sq, const enum square to_sq) {
    struct move mov = {0};
    mov.val = ENCODE_FROM_SQ(from_sq) | ENCODE_TO_SQ(to_sq);

    return mov;
}

__attribute__((always_inline)) uint16_t set_flag(const uint16_t mv, const uint16_t flag_bit) {
    return mv | flag_bit;
}
