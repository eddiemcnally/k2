#include <stdint.h>
#include <assert.h>
#include "piece.h"
#include "square.h"
#include "move.h"

/**
 * bitmap for the struct
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
struct move
{
    uint16_t mv;
};

enum move_bits_masks
{
    MV_MASK_TO_SQ = 0x003f,
    MV_MASK_FROM_SQ = 0x0FC0,
    MV_MASK_FLAGS = 0xC000,
};

enum move_bits_shifts
{
    MV_SHFT_TO_SQ = 0,
    MV_SHFT_FROM_SQ = 6,
    MV_SHFT_FLAGS = 12
};

enum move_flags
{
    MV_FLG_QUIET = 0x0,
    MV_FLG_DOUBLE_PAWN = 0x1,
    MV_FLG_KING_CASTLE = 0x2,
    MV_FLG_QUEEN_CASTLE = 0x3,
    MV_FLG_CAPTURE = 0x4,
    MV_FLG_EN_PASS = 0x5,
    MV_FLG_PROMOTE_KNIGHT = 0x8,
    MV_FLG_PROMOTE_BISHOP = 0x9,
    MV_FLG_PROMOTE_ROOK = 0xA,
    MV_FLG_PROMOTE_QUEEN = 0xB,
    MV_FLG_PROMOTE_KNIGHT_CAPTURE = 0xC,
    MV_FLG_PROMOTE_BISHOP_CAPTURE = 0xD,
    MV_FLG_PROMOTE_ROOK_CAPTURE = 0xE,
    MV_FLG_PROMOTE_QUEEN_CAPTURE = 0xF,
};

enum move_flag_bits
{
    MV_FLG_BIT_PROMOTE = 0x8,
    MV_FLG_BIT_CAPTURE = 0x4,
};


static void encode_to_from(struct move *mv, const enum square from_sq, const enum square to_sq);

struct move encode_move_quiet(const enum square from_sq, const enum square to_sq)
{
    struct move mv = {0};
    encode_to_from(&mv, from_sq, to_sq);

    return mv;
}

struct move encode_move_promoted(const enum square from_sq, const enum square to_sq, const enum piece promoted_piece, const bool is_capture)
{
    struct move mv = {0};

    encode_to_from(&mv, from_sq, to_sq);

    uint16_t mask = 0;

    switch (promoted_piece) {
    case WKNIGHT:
    case BKNIGHT:
        mask = MV_FLG_PROMOTE_KNIGHT;
        break;
    case WBISHOP:
    case BBISHOP:
        mask = MV_FLG_PROMOTE_BISHOP;
        break;
    case WROOK:
    case BROOK:
        mask = MV_FLG_PROMOTE_ROOK;
        break;
    case WQUEEN:
    case BQUEEN:
        mask = MV_FLG_PROMOTE_QUEEN;
        break;
    default:
        assert(false);
        break;
    }

    if (is_capture) {
        mask = mask | (uint16_t) MV_FLG_BIT_CAPTURE;
    }

    mask = (uint16_t) (mask << (uint16_t) MV_SHFT_FLAGS);

    mv.mv = mv.mv | mask;

    return mv;
}

struct move encode_move_capture(const enum square from_sq, const enum square to_sq)
{
    struct move mv = {0};
    encode_to_from(&mv, from_sq, to_sq);
    mv.mv = mv.mv | (MV_FLG_BIT_CAPTURE << MV_SHFT_FLAGS);
    return mv;
}

struct move encode_move_enpassant(const enum square from_sq, const enum square to_sq)
{
    struct move mv = {0};
    encode_to_from(&mv, from_sq, to_sq);
    mv.mv = mv.mv | (MV_FLG_EN_PASS << MV_SHFT_FLAGS);
    return mv;
}

enum square decode_move_from_sq(const struct move mv)
{
    return (enum square)((mv.mv & MV_MASK_FROM_SQ) >> MV_SHFT_FROM_SQ);
}

enum square decode_move_to_sq(const struct move mv)
{
    return (enum square)((mv.mv & MV_MASK_TO_SQ) >> MV_SHFT_TO_SQ);
}

static void encode_to_from(struct move *mv, const enum square from_sq, const enum square to_sq)
{
    uint16_t m = (uint16_t) (from_sq << MV_SHFT_FROM_SQ);
    mv->mv = mv->mv | (uint16_t) (m & MV_MASK_FROM_SQ);

    m = (uint16_t) (to_sq << MV_SHFT_TO_SQ);
    mv->mv = mv->mv | (m & (uint16_t) MV_MASK_TO_SQ);
}
