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
 * See http://chessprogramming.wikispaces.com/Encoding+Moves
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

enum move_flags {
    MV_FLG_QUIET = 0x0000,
    MV_FLG_DOUBLE_PAWN = 0x1000,
    MV_FLG_KING_CASTLE = 0x2000,
    MV_FLG_QUEEN_CASTLE = 0x3000,
    MV_FLG_CAPTURE = 0x4000,
    MV_FLG_EN_PASS = 0x5000,
    MV_FLG_PROMOTE_KNIGHT = 0x8000,
    MV_FLG_PROMOTE_BISHOP = 0x9000,
    MV_FLG_PROMOTE_ROOK = 0xA000,
    MV_FLG_PROMOTE_QUEEN = 0xB000,
    MV_FLG_PROMOTE_KNIGHT_CAPTURE = 0xC000,
    MV_FLG_PROMOTE_BISHOP_CAPTURE = 0xD000,
    MV_FLG_PROMOTE_ROOK_CAPTURE = 0xE000,
    MV_FLG_PROMOTE_QUEEN_CAPTURE = 0xF000,
};

enum move_flag_bits {
    MV_FLG_BIT_PROMOTE = 0x8000,
    MV_FLG_BIT_CAPTURE = 0x4000,
};

static uint16_t set_flag(const uint16_t mv, const uint16_t flag);
static struct move encode_from_to(const enum square from_sq, const enum square to_sq);
static const char *move_details(const struct move mv);

// ==================================================================
//
// public functions
//
// ==================================================================

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
        printf("Invalid promotion piece\n");
        exit(-1);
        break;
    }

    mv.val = set_flag(mv.val, flg);
    if (is_capture) {
        mv.val = set_flag(mv.val, MV_FLG_BIT_CAPTURE);
    }

    return mv;
}

/**
 * @brief Decodes the promotion piece from the move
 * 
 * @param mv 
 * @param side 
 * @param pce       ptr to piece to set up
 * @return true     ok
 * @return false    not ok
 */
bool try_move_decode_promotion_piece(const struct move mv, const enum colour side, enum piece *pce) {
    assert(validate_move(mv));
    assert(validate_colour(side));

    const uint16_t m = mv.val & MV_MASK_FLAGS;

    switch (m) {
    case MV_FLG_PROMOTE_KNIGHT_CAPTURE:
    case MV_FLG_PROMOTE_KNIGHT:
        if (side == WHITE) {
            *pce = WHITE_KNIGHT;
        } else {
            *pce = BLACK_KNIGHT;
        }
        return true;
    case MV_FLG_PROMOTE_BISHOP_CAPTURE:
    case MV_FLG_PROMOTE_BISHOP:
        if (side == WHITE) {
            *pce = WHITE_BISHOP;
        } else {
            *pce = BLACK_BISHOP;
        }
        return true;
    case MV_FLG_PROMOTE_QUEEN_CAPTURE:
    case MV_FLG_PROMOTE_QUEEN:
        if (side == WHITE) {
            *pce = WHITE_QUEEN;
        } else {
            *pce = BLACK_QUEEN;
        }
        return true;
    case MV_FLG_PROMOTE_ROOK_CAPTURE:
    case MV_FLG_PROMOTE_ROOK:
        if (side == WHITE) {
            *pce = WHITE_ROOK;
        } else {
            *pce = BLACK_ROOK;
        }
        return true;
    default:
        assert(false);
    }

    assert(false);
    return false;
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
    enum square from_sq;
    enum square to_sq;

    switch (side) {
    case WHITE:
        from_sq = e1;
        to_sq = g1;
        break;
    case BLACK:
        from_sq = e8;
        to_sq = g8;
        break;
    default:
        assert(false);
    }

    struct move mv = encode_from_to(from_sq, to_sq);
    mv.val = set_flag(mv.val, MV_FLG_KING_CASTLE);
    return mv;
}

/**
 * @brief       Encodes a Queen-side castle move
 *
 * @return      The encoded move
 */
struct move move_encode_castle_queenside(const enum colour side) {
    enum square from_sq;
    enum square to_sq;

    switch (side) {
    case WHITE:
        from_sq = e1;
        to_sq = c1;
        break;
    case BLACK:
        from_sq = e8;
        to_sq = c8;
        break;
    default:
        assert(false);
    }

    struct move mv = encode_from_to(from_sq, to_sq);
    mv.val = set_flag(mv.val, MV_FLG_QUEEN_CASTLE);
    return mv;
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

static inline struct move encode_from_to(const enum square from_sq, const enum square to_sq) {
    uint16_t mv = 0;

    uint16_t m = (uint16_t)(from_sq << MV_SHFT_FROM_SQ);
    mv |= (uint16_t)(m & MV_MASK_FROM_SQ);

    m = (uint16_t)(to_sq << MV_SHFT_TO_SQ);
    mv |= (uint16_t)(m & MV_MASK_TO_SQ);

    struct move mov = {.val = mv};
    return mov;
}

static inline uint16_t set_flag(const uint16_t mv, const uint16_t flag_bit) {
    return mv | flag_bit;
}
