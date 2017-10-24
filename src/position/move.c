#include <stdint.h>
#include <assert.h>
#include "piece.h"
#include "square.h"
#include "move.h"

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

static void set_flag(struct move *mv, uint16_t flag);
static struct move encode_to_from(const enum square from_sq, const enum square to_sq);

struct move move_encode_quiet(const enum square from_sq, const enum square to_sq)
{
    return encode_to_from(from_sq, to_sq);
}

struct move move_encode_promoted(const enum square from_sq, const enum square to_sq, const enum piece promoted_piece, const bool is_capture)
{
    struct move mv = encode_to_from(from_sq, to_sq);

    enum piece pce_type = pce_get_piece_type(promoted_piece);

    switch (pce_type) {
    case KNIGHT:
        set_flag(&mv, MV_FLG_PROMOTE_KNIGHT);
        break;
    case BISHOP:
        set_flag(&mv, MV_FLG_PROMOTE_BISHOP);
        break;
    case ROOK:
        set_flag(&mv, MV_FLG_PROMOTE_ROOK);
        break;
    case QUEEN:
        set_flag(&mv, MV_FLG_PROMOTE_QUEEN);
        break;
    default:
        assert(false);
        break;
    }

    if (is_capture) {
        set_flag(&mv, MV_FLG_BIT_CAPTURE);
    }

    return mv;
}

struct move move_encode_capture(const enum square from_sq, const enum square to_sq)
{
    struct move mv = encode_to_from(from_sq, to_sq);
    set_flag(&mv, MV_FLG_CAPTURE);
    return mv;
}

struct move move_encode_enpassant(const enum square from_sq, const enum square to_sq)
{
    struct move mv = encode_to_from(from_sq, to_sq);
    set_flag(&mv, MV_FLG_EN_PASS);
    return mv;
}

enum square move_decode_from_sq(const struct move *mv)
{
    return (enum square)((mv->mv & MV_MASK_FROM_SQ) >> MV_SHFT_FROM_SQ);
}

enum square move_decode_to_sq(const struct move *mv)
{
    return (enum square)((mv->mv & MV_MASK_TO_SQ) >> MV_SHFT_TO_SQ);
}

static struct move encode_to_from(const enum square from_sq, const enum square to_sq)
{
    struct move mv = {.mv = 0};
    uint16_t m = (uint16_t) (from_sq << MV_SHFT_FROM_SQ);
    mv.mv |= (uint16_t) (m & MV_MASK_FROM_SQ);

    m = (uint16_t) (to_sq << MV_SHFT_TO_SQ);
    mv.mv |= (uint16_t) (m & MV_MASK_TO_SQ);

    return mv;
}

static void set_flag(struct move *mv, uint16_t flag_bit)
{
    mv->mv |= flag_bit;
}
