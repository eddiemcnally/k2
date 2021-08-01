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

/*! @addtogroup PositionManagement
 *
 * @ingroup PositionManagement
 * @{
 * @details Functions to encode the rules of the game (making/taking moves, handling en passant, castling, etc)
 *
 */

#include "position.h"
#include "attack_checker.h"
#include "board.h"
#include "castle_perms.h"
#include "fen.h"
#include "hashkeys.h"
#include "move.h"
#include "occupancy_mask.h"
#include "utils.h"
#include <assert.h>

// key used to verify struct has been initialised
const static uint16_t STRUCT_INIT_KEY = 0xdead;

struct game_state {
    // position hash
    uint64_t hashkey;

    // the next side to move
    enum colour side_to_move;

    // keeping track of ply
    uint16_t ply;         // half-moves
    uint16_t history_ply; // full move count

    uint8_t fifty_move_counter;

    enum square en_passant_sq;

    // active catle permissions
    struct cast_perm_container castle_perm_container;
};

struct history_item {
    struct game_state state;
    struct move mv;
    enum piece pce_moved;
    enum piece captured_piece;
};

struct position_history {
    struct history_item items[MAX_GAME_MOVES];
    uint16_t num_used_slots;
};

// represents the current game position
struct position {
    struct game_state state;

    // current board representation
    struct board *brd;

    // position history
    struct position_history history;

    uint16_t struct_init_key;
};

static void init_pos_struct(struct position *const pos);
static void populate_position_from_fen(struct position *const pos, const struct parsed_fen *fen);
static void set_up_castle_permissions(struct position *const pos, const struct parsed_fen *fen);
static enum square get_en_pass_sq(const enum colour side, const enum square from_sq);
static void do_capture_move(struct position *const pos, const enum square from_sq, const enum square to_sq,
                            const enum piece pce_to_move);
static void make_king_side_castle_move(struct position *const pos);
static void make_queen_side_castle_move(struct position *const pos);
static void make_en_passant_move(struct position *const pos, const enum square from_sq, const enum square to_sq);
static enum move_legality get_move_legal_status(const struct position *const pos, const struct move mov);
static bool is_castle_move_legal(const struct position *const pos, const struct move mov,
                                 const enum colour side_to_move, const enum colour attacking_side);
static void update_castle_perms(struct position *const pos, const struct move mv, const enum piece pce_being_moved);
static void pos_move_piece(struct position *const pos, const enum piece pce, const enum square from_sq,
                           const enum square to_sq);
static void pos_remove_piece(struct position *const pos, const enum piece pce, const enum square sq);
static void pos_add_piece(struct position *const pos, const enum piece pce, const enum square sq);
static void pos_update_castle_perm(struct position *const pos, const enum castle_permission perm,
                                   const bool perm_state);
static void swap_side(struct position *const pos);
static bool pos_is_en_passant_active(const struct position *const pos);
static void do_promotion_quiet(struct position *const pos, const enum piece pce_to_move, const enum square from_sq,
                               const enum square to_sq, const enum piece target_promotion_pce);
static void do_promotion_capture(struct position *const pos, const enum piece pce_to_move, const enum square from_sq,
                                 const enum square to_sq, const enum piece target_promotion_pce);
static void reverse_quiet_move(struct position *const pos, const struct move mv, const enum piece pce_moved);
static void reverse_capture_move(struct position *const pos, const struct move mv, const enum piece pce_moved,
                                 const enum piece captured_piece);
static void reverse_quiet_promotion_move(struct position *const pos, const struct move mv, const enum piece pce_moved);
static void reverse_capture_promotion_move(struct position *const pos, const struct move mv, const enum piece pce_moved,
                                           const enum piece captured_piece);
static void reverse_en_passant_move(struct position *const pos, const struct move mv, const enum colour side);
static void reverse_castle_move(struct position *const pos, const struct move mv, const enum colour side);

static void position_hist_push(struct position *const pos, const struct move mv, const enum piece pce_moved,
                               const enum piece captured_piece);
static void position_hist_pop(struct position *const pos, struct move *mv, enum piece *pce_moved,
                              enum piece *captured_piece);
static bool validate_position_history(const struct position *const pos);
static bool position_hist_compare(const struct position *const pos1, const struct position *const pos2);
static bool compare_game_states(const struct game_state *gs1, const struct game_state *gs2);

// set up bitboards for all squares that need to be checked when
// testing castle move validity
static const uint64_t WK_CAST_BB = (uint64_t)((0x01L << e1) | (0x01L << f1) | (0x01L << g1));
static const uint64_t BK_CAST_BB = (uint64_t)((0x01L << e8) | (0x01L << f8) | (0x01L << g8));
static const uint64_t WQ_CAST_BB = (uint64_t)((0x01L << c1) | (0x01L << d1) | (0x01L << e1));
static const uint64_t BQ_CAST_BB = (uint64_t)((0x01L << c8) | (0x01L << d8) | (0x01L << e8));

/**
 * @brief       Create and initialise an empty instance of the Position struct
 *
 * @return      An initialised Position struct
 */
struct position *pos_create() {
    struct position *retval = (struct position *)malloc(sizeof(struct position));
    init_pos_struct(retval);

    struct board *brd = brd_allocate();
    retval->brd = brd;

    init_key_mgmt();
    occ_mask_init();

    return retval;
}

/**
 * @brief       Sets up the position using the goven FEN
 *
 * @param fen   The FEN string
 * @param pos   The position struct
 */
void pos_initialise(const char *fen, struct position *const pos) {
    struct parsed_fen *parsed_fen = fen_parse(fen);
    populate_position_from_fen(pos, parsed_fen);
}

/**
 * @brief       Cleans up the Position and frees up any memory
 *
 * @param pos   The position to destroy
 */
void pos_destroy(struct position *const pos) {
    assert(validate_position(pos));
    brd_deallocate(pos->brd);

    memset(pos, 0, sizeof(struct position));
    free(pos);
}

/**
 * @brief       Returns the board managed by this position
 *
 * @param pos   The Position
 * @return      The board
 */
struct board *pos_get_board(const struct position *const pos) {
    assert(validate_position(pos));
    return pos->brd;
}

uint16_t pos_get_ply(const struct position *const pos) {
    return pos->state.ply;
}

/**
 * @brief       Gets the side to move from the position struct
 *
 * @param pos   The position
 * @return      Side to move
 */
enum colour pos_get_side_to_move(const struct position *const pos) {
    assert(validate_position(pos));
    return pos->state.side_to_move;
}

/**
 * @brief       Returns the current Castle Permissions
 *
 * @param pos   The position
 * @return      The castle permissions available
 */
struct cast_perm_container pos_get_cast_perm(const struct position *const pos) {
    return pos->state.castle_perm_container;
}

/**
 * @brief               Tries to return the en passant square
 *
 * @param pos           The position
 * @param en_pass_sq
 * @return              true if valid/active en passent square, false otherwise
 */
enum square pos_get_en_pass_sq(const struct position *const pos) {
    if (pos_is_en_passant_active(pos)) {
        return pos->state.en_passant_sq;
    }
    return NO_SQUARE;
}

/**
 * @brief               Sets the current Castle permissions
 *
 * @param pos           The position
 * @param perms         Castle permissions to set
 */
void pos_set_cast_perm(struct position *const pos, const struct cast_perm_container perms) {
    pos->state.castle_perm_container = perms;
}

/**
 * @brief               Validates the current position
 *
 * @param pos           The position
 * @param perms         true if position is valid/consistent, false otherwise
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool validate_position(const struct position *const pos) {
    assert(pos->struct_init_key == STRUCT_INIT_KEY);

    assert(validate_board(pos->brd));
    assert(validate_colour(pos->state.side_to_move));

    return true;
}
#pragma GCC diagnostic pop

enum move_legality pos_make_move(struct position *const pos, const struct move mv) {
    assert(validate_position(pos));
    assert(validate_move(mv));

    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);
    const enum piece pce_to_move = brd_get_piece_on_square(pos->brd, from_sq);
    const enum piece pce_capt = brd_get_piece_on_square(pos->brd, to_sq);

    position_hist_push(pos, mv, pce_to_move, pce_capt);

    pos->state.ply++;
    pos->state.history_ply++;

    assert(validate_piece(pce_to_move));

    const enum move_type mv_type = move_get_move_type(mv);
    switch (mv_type) {
    case MV_TYPE_QUIET:
        pos_move_piece(pos, pce_to_move, from_sq, to_sq);
        break;
    case MV_TYPE_CAPTURE:
        do_capture_move(pos, from_sq, to_sq, pce_to_move);
        break;
    case MV_TYPE_DOUBLE_PAWN:
        pos->state.en_passant_sq = get_en_pass_sq(pos->state.side_to_move, from_sq);
        pos_move_piece(pos, pce_to_move, from_sq, to_sq);
        break;
    case MV_TYPE_EN_PASS:
        pos->state.en_passant_sq = NO_SQUARE;
        make_en_passant_move(pos, from_sq, to_sq);
        break;
    case MV_TYPE_QUEEN_CASTLE:
        make_queen_side_castle_move(pos);
        break;
    case MV_TYPE_KING_CASTLE:
        make_king_side_castle_move(pos);
        break;
    case MV_TYPE_PROMOTE_BISHOP: {
        const enum piece pce_prom = pos->state.side_to_move == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
        do_promotion_quiet(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_KNIGHT: {
        const enum piece pce_prom = pos->state.side_to_move == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
        do_promotion_quiet(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_QUEEN: {
        const enum piece pce_prom = pos->state.side_to_move == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
        do_promotion_quiet(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_ROOK: {
        const enum piece pce_prom = pos->state.side_to_move == WHITE ? WHITE_ROOK : BLACK_ROOK;
        do_promotion_quiet(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_BISHOP_CAPTURE: {
        const enum piece pce_prom = pos->state.side_to_move == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
        do_promotion_capture(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_KNIGHT_CAPTURE: {
        const enum piece pce_prom = pos->state.side_to_move == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
        do_promotion_capture(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_QUEEN_CAPTURE: {
        const enum piece pce_prom = pos->state.side_to_move == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
        do_promotion_capture(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_ROOK_CAPTURE: {
        const enum piece pce_prom = pos->state.side_to_move == WHITE ? WHITE_ROOK : BLACK_ROOK;
        do_promotion_capture(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    default:
        REQUIRE(false, "Invalid move type");
    }

    // some cleanup
    const enum move_legality legality = get_move_legal_status(pos, mv);
    if (move_is_double_pawn(mv) == false) {
        pos->state.en_passant_sq = NO_SQUARE;
    }
    // clean out any castle permissions
    update_castle_perms(pos, mv, pce_to_move);

    swap_side(pos);

    return legality;
}

struct move pos_take_move(struct position *const pos) {
    assert(validate_position(pos));

    struct move mv = {0};
    enum piece pce_moved;
    enum piece captured_piece;

    position_hist_pop(pos, &mv, &pce_moved, &captured_piece);

    const enum move_type mv_type = move_get_move_type(mv);

    switch (mv_type) {
    case MV_TYPE_QUIET:
        reverse_quiet_move(pos, mv, pce_moved);
        break;
    case MV_TYPE_CAPTURE:
        reverse_capture_move(pos, mv, pce_moved, captured_piece);
        break;
    case MV_TYPE_DOUBLE_PAWN:
        reverse_quiet_move(pos, mv, pce_moved);
        break;
    case MV_TYPE_EN_PASS:
        reverse_en_passant_move(pos, mv, pos->state.side_to_move);
        break;
    case MV_TYPE_QUEEN_CASTLE:
        reverse_castle_move(pos, mv, pos->state.side_to_move);
        break;
    case MV_TYPE_KING_CASTLE:
        reverse_castle_move(pos, mv, pos->state.side_to_move);
        break;
    case MV_TYPE_PROMOTE_BISHOP:
    case MV_TYPE_PROMOTE_KNIGHT:
    case MV_TYPE_PROMOTE_QUEEN:
    case MV_TYPE_PROMOTE_ROOK:
        reverse_quiet_promotion_move(pos, mv, pce_moved);
        break;
    case MV_TYPE_PROMOTE_BISHOP_CAPTURE:
    case MV_TYPE_PROMOTE_KNIGHT_CAPTURE:
    case MV_TYPE_PROMOTE_QUEEN_CAPTURE:
    case MV_TYPE_PROMOTE_ROOK_CAPTURE:
        reverse_capture_promotion_move(pos, mv, pce_moved, captured_piece);
        break;
    default:
        REQUIRE(false, "Invalid move type");
    }

    return mv;
}

static void reverse_quiet_move(struct position *const pos, const struct move mv, const enum piece pce_moved) {
    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);
    brd_move_piece(pos->brd, pce_moved, to_sq, from_sq);
}

static void reverse_capture_move(struct position *const pos, const struct move mv, const enum piece pce_moved,
                                 const enum piece captured_piece) {

    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);
    brd_move_piece(pos->brd, pce_moved, to_sq, from_sq);
    brd_add_piece(pos->brd, captured_piece, to_sq);
}

static void reverse_quiet_promotion_move(struct position *const pos, const struct move mv, const enum piece pce_moved) {

    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);

    // remove promoted piece
    brd_remove_from_square(pos->brd, to_sq);
    // put piece back to its original square
    brd_add_piece(pos->brd, pce_moved, from_sq);
}

static void reverse_capture_promotion_move(struct position *const pos, const struct move mv, const enum piece pce_moved,
                                           const enum piece captured_piece) {
    assert(captured_piece != NO_PIECE);

    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);

    // remove promoted piece and add the captured piece
    brd_remove_from_square(pos->brd, to_sq);
    brd_add_piece(pos->brd, captured_piece, to_sq);

    // put piece back to its original square
    brd_add_piece(pos->brd, pce_moved, from_sq);
}

static void reverse_en_passant_move(struct position *const pos, const struct move mv, const enum colour side) {
    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);

    switch (side) {
    case WHITE:
        // note: to/from swapped
        brd_move_piece(pos->brd, WHITE_PAWN, to_sq, from_sq);
        const enum square wcapture_sq = sq_get_square_minus_1_rank(to_sq);
        brd_add_piece(pos->brd, BLACK_PAWN, wcapture_sq);
        break;
    case BLACK:
        // note: to/from swapped
        brd_move_piece(pos->brd, BLACK_PAWN, to_sq, from_sq);
        const enum square bcapture_sq = sq_get_square_plus_1_rank(to_sq);
        brd_add_piece(pos->brd, WHITE_PAWN, bcapture_sq);
        break;
    default:
        REQUIRE(false, "Invalid side : reverse en passant")
    }
}

static void reverse_castle_move(struct position *const pos, const struct move mv, const enum colour side) {
    switch (side) {
    case WHITE:
        if (move_is_king_castle(mv)) {
            brd_move_piece(pos->brd, WHITE_KING, g1, e1);
            brd_move_piece(pos->brd, WHITE_ROOK, f1, h1);
        } else {
            brd_move_piece(pos->brd, WHITE_KING, c1, e1);
            brd_move_piece(pos->brd, WHITE_ROOK, d1, a1);
        }
        break;
    case BLACK:
        if (move_is_king_castle(mv)) {
            brd_move_piece(pos->brd, BLACK_KING, g8, e8);
            brd_move_piece(pos->brd, BLACK_ROOK, f8, h8);
        } else {
            brd_move_piece(pos->brd, BLACK_KING, c8, e8);
            brd_move_piece(pos->brd, BLACK_ROOK, d8, a8);
        }
        break;
    }
}

static void do_promotion_quiet(struct position *const pos, const enum piece pce_to_move, const enum square from_sq,
                               const enum square to_sq, const enum piece target_promotion_pce) {
    pos_remove_piece(pos, pce_to_move, from_sq);
    pos_add_piece(pos, target_promotion_pce, to_sq);
}

static void do_promotion_capture(struct position *const pos, const enum piece pce_to_move, const enum square from_sq,
                                 const enum square to_sq, const enum piece target_promotion_pce) {

    enum piece pce_being_captured = brd_get_piece_on_square(pos->brd, to_sq);
    pos_remove_piece(pos, pce_being_captured, to_sq);

    pos_remove_piece(pos, pce_to_move, from_sq);
    pos_add_piece(pos, target_promotion_pce, to_sq);
}

static void swap_side(struct position *const pos) {
    pos->state.side_to_move = pce_swap_side(pos->state.side_to_move);
    pos->state.hashkey = hash_side_update(pos->state.hashkey);
}

/**
 * @brief               Compares 2 positions for equivalency
 * @param first         The first position
 * @param second        The second position
 * @return              True if the positions are the same, false otherwise
 */
bool pos_compare(const struct position *first, const struct position *second) {
    assert(validate_position(first));
    assert(validate_position(second));

    if (brd_compare(first->brd, second->brd) == false) {
        printf("pos_compare: Board compare failed\n");
        return false;
    }

    if (first->state.side_to_move != second->state.side_to_move) {
        printf("pos_compare: side to move is different\n");
        return false;
    }

    if (first->state.ply != second->state.ply) {
        printf("pos_compare: ply is different\n");
        return false;
    }

    if (first->state.history_ply != second->state.history_ply) {
        printf("pos_compare: history_ply is different\n");
        return false;
    }

    if (first->state.fifty_move_counter != second->state.fifty_move_counter) {
        printf("pos_compare: 50 move counter is different\n");
        return false;
    }
    if (cast_compare_perms(first->state.castle_perm_container, second->state.castle_perm_container) == false) {
        printf("pos_compare: castle permissions are different\n");
        return false;
    }

    if (first->state.en_passant_sq != second->state.en_passant_sq) {
        printf("pos_compare: en passant state is different\n");
        return false;
    }

    bool same_hist = position_hist_compare(first, second);
    if (same_hist == false) {
        printf("pos_compare: position history is different\n");
        return false;
    }

    return true;
}

uint64_t pos_get_hash(const struct position *const pos) {
    return pos->state.hashkey;
}

// ==================================================================
//
// private functions
//
// ==================================================================

static inline bool pos_is_en_passant_active(const struct position *const pos) {
    return pos->state.en_passant_sq != NO_SQUARE;
}

static void init_pos_struct(struct position *const pos) {
    memset(pos, 0, sizeof(struct position));
    pos->struct_init_key = STRUCT_INIT_KEY;

    pos->state.castle_perm_container = cast_perm_init();

    pos->history.num_used_slots = 0;
}

static void do_capture_move(struct position *const pos, const enum square from_sq, const enum square to_sq,
                            const enum piece pce_to_move) {
    enum piece pce_capt = brd_get_piece_on_square(pos->brd, to_sq);

    pos_remove_piece(pos, pce_capt, to_sq);
    pos_move_piece(pos, pce_to_move, from_sq, to_sq);
}

static void update_castle_perms(struct position *const pos, const struct move mv, const enum piece pce_being_moved) {
    if (move_is_castle(mv)) {
        // already handled elsewhere
        return;
    }

    const enum piece_role pce_role = pce_get_piece_role(pce_being_moved);
    const enum square to_sq = move_decode_to_sq(mv);
    const enum square from_sq = move_decode_from_sq(mv);

    if (pce_role == KING) {
        // king moved, reset castle permissions
        const enum colour side = pce_get_colour(pce_being_moved);
        if (side == WHITE) {
            pos_update_castle_perm(pos, CASTLE_PERM_WK, false);
            pos_update_castle_perm(pos, CASTLE_PERM_WQ, false);
        } else {
            pos_update_castle_perm(pos, CASTLE_PERM_BK, false);
            pos_update_castle_perm(pos, CASTLE_PERM_BQ, false);
        }
    } else if (pce_role == ROOK) {
        // rook moved, reset castle permissions
        const enum colour side = pce_get_colour(pce_being_moved);
        if (side == WHITE) {
            switch (from_sq) {
            case a1:
                pos_update_castle_perm(pos, CASTLE_PERM_WQ, false);
                break;
            case h1:
                pos_update_castle_perm(pos, CASTLE_PERM_WK, false);
                break;
            default:
                break;
            }
        } else {
            switch (from_sq) {
            case a8:
                pos_update_castle_perm(pos, CASTLE_PERM_BQ, false);
                break;
            case h8:
                pos_update_castle_perm(pos, CASTLE_PERM_BK, false);
                break;
            default:
                break;
            }
        }
    }

    if (move_is_capture(mv)) {
        switch (to_sq) {
        case a8:
            pos_update_castle_perm(pos, CASTLE_PERM_BQ, false);
            break;
        case h8:
            pos_update_castle_perm(pos, CASTLE_PERM_BK, false);
            break;
        case a1:
            pos_update_castle_perm(pos, CASTLE_PERM_WQ, false);
            break;
        case h1:
            pos_update_castle_perm(pos, CASTLE_PERM_WK, false);
            break;
        default:
            break;
        }
    }
}

static enum move_legality get_move_legal_status(const struct position *const pos, const struct move mov) {
    const enum colour side_to_move = pos_get_side_to_move(pos);
    const enum colour attacking_side = pce_swap_side(side_to_move);

    const enum piece king = side_to_move == WHITE ? WHITE_KING : BLACK_KING;

    uint64_t king_bb = brd_get_piece_bb(pos->brd, king);
    const enum square king_sq = bb_pop_1st_bit(king_bb);

    if (att_chk_is_sq_attacked(pos->brd, king_sq, attacking_side)) {
        // square attacked, move not legal
        return ILLEGAL_MOVE;
    }

    if (move_is_castle(mov)) {
        // check that the castle wasn't done through attacked squares
        if (is_castle_move_legal(pos, mov, side_to_move, attacking_side) == false) {
            return ILLEGAL_MOVE;
        }
    }

    return LEGAL_MOVE;
}

static bool is_castle_move_legal(const struct position *const pos, const struct move mov,
                                 const enum colour side_to_move, const enum colour attacking_side) {
    uint64_t cast_bb = 0;
    assert(move_is_castle(mov));

    if (move_is_king_castle(mov)) {
        if (side_to_move == WHITE) {
            cast_bb = WK_CAST_BB;
        } else {
            cast_bb = BK_CAST_BB;
        }
    } else if (move_is_queen_castle(mov)) {
        if (side_to_move == WHITE) {
            cast_bb = WQ_CAST_BB;
        } else {
            cast_bb = BQ_CAST_BB;
        }
    } else {
        REQUIRE(false, "is_castle_move_legal : unexpected condition");
    }

    while (cast_bb != 0) {
        const enum square sq = bb_pop_1st_bit(cast_bb);
        bb_clear_square(&cast_bb, sq);
        if (att_chk_is_sq_attacked(pos->brd, sq, attacking_side)) {
            return false;
        }
    }
    return true;
}

static void make_king_side_castle_move(struct position *const pos) {
    switch (pos->state.side_to_move) {
    case WHITE:
        pos_move_piece(pos, WHITE_KING, e1, g1);
        pos_move_piece(pos, WHITE_ROOK, h1, f1);

        pos_update_castle_perm(pos, CASTLE_PERM_WK, false);
        pos_update_castle_perm(pos, CASTLE_PERM_WQ, false);
        break;
    case BLACK:
        pos_move_piece(pos, BLACK_KING, e8, g8);
        pos_move_piece(pos, BLACK_ROOK, h8, f8);

        pos_update_castle_perm(pos, CASTLE_PERM_BK, false);
        pos_update_castle_perm(pos, CASTLE_PERM_BQ, false);
        break;
    default:
        REQUIRE(false, "Unexpected condition");
    }
}

static void make_queen_side_castle_move(struct position *const pos) {
    switch (pos->state.side_to_move) {
    case WHITE:
        pos_move_piece(pos, WHITE_KING, e1, c1);
        pos_move_piece(pos, WHITE_ROOK, a1, d1);
        pos_update_castle_perm(pos, CASTLE_PERM_WK, false);
        pos_update_castle_perm(pos, CASTLE_PERM_WQ, false);
        break;
    case BLACK:
        pos_move_piece(pos, BLACK_KING, e8, c8);
        pos_move_piece(pos, BLACK_ROOK, a8, d8);
        pos_update_castle_perm(pos, CASTLE_PERM_BK, false);
        pos_update_castle_perm(pos, CASTLE_PERM_BQ, false);
        break;
    default:
        REQUIRE(false, "Unexpected condition");
    }
}

static void make_en_passant_move(struct position *const pos, const enum square from_sq, const enum square to_sq) {

    const enum piece pce_to_move = brd_get_piece_on_square(pos->brd, from_sq);
    const enum square sq_with_piece =
        pos->state.side_to_move == WHITE ? sq_get_square_minus_1_rank(to_sq) : sq_get_square_plus_1_rank(to_sq);
    const enum piece pce_to_remove = brd_get_piece_on_square(pos->brd, sq_with_piece);

    pos_remove_piece(pos, pce_to_remove, sq_with_piece);
    pos_move_piece(pos, pce_to_move, from_sq, to_sq);
    pos->state.hashkey = hash_en_passant(to_sq, pos->state.hashkey);
}

static void populate_position_from_fen(struct position *const pos, const struct parsed_fen *fen) {
    pos->state.side_to_move = fen_get_side_to_move(fen);

    enum square en_pass;
    bool found_en_pass = fen_try_get_en_pass_sq(fen, &en_pass);
    if (found_en_pass) {
        pos->state.en_passant_sq = en_pass;
    } else {
        pos->state.en_passant_sq = NO_SQUARE;
    }

    pos->state.fifty_move_counter = 0;
    pos->state.ply = fen_get_half_move_cnt(fen);
    pos->state.history_ply = fen_get_full_move_cnt(fen);
    set_up_castle_permissions(pos, fen);

    for (enum square sq = a1; sq <= h8; sq++) {
        enum piece pce;
        bool found_pce = fen_try_get_piece_on_sq(fen, sq, &pce);
        if (found_pce) {
            pos_add_piece(pos, pce, sq);
        }
    }
}

static void set_up_castle_permissions(struct position *const pos, const struct parsed_fen *fen) {

    pos_update_castle_perm(pos, CASTLE_PERM_NONE, true);

    if (fen_has_wk_castle_perms(fen)) {
        pos_update_castle_perm(pos, CASTLE_PERM_WK, true);
    }
    if (fen_has_wq_castle_perms(fen)) {
        pos_update_castle_perm(pos, CASTLE_PERM_WQ, true);
    }
    if (fen_has_bk_castle_perms(fen)) {
        pos_update_castle_perm(pos, CASTLE_PERM_BK, true);
    }
    if (fen_has_bq_castle_perms(fen)) {
        pos_update_castle_perm(pos, CASTLE_PERM_BQ, true);
    }
}

static void pos_update_castle_perm(struct position *const pos, const enum castle_permission perm,
                                   const bool perm_state) {
    cast_perm_set_permission(perm, &pos->state.castle_perm_container, perm_state);
    pos->state.hashkey = hash_castle_perm(perm, pos->state.hashkey);
}

static enum square get_en_pass_sq(const enum colour side, const enum square from_sq) {

    const enum square retval = side == WHITE ? sq_get_square_plus_1_rank(from_sq) : sq_get_square_minus_1_rank(from_sq);
    assert(validate_en_pass_sq(retval));

    return retval;
}

static void position_hist_push(struct position *const pos, const struct move mv, const enum piece pce_moved,
                               const enum piece captured_piece) {

    assert(validate_position_history(pos));
    assert(validate_move(mv));
    assert(validate_piece(pce_moved));
    assert(validate_piece(captured_piece));

    struct history_item *free_slot = &pos->history.items[pos->history.num_used_slots];

    __builtin_memcpy_inline(&free_slot->state, &pos->state, sizeof(struct game_state));

    free_slot->mv = mv;
    free_slot->pce_moved = pce_moved;
    free_slot->captured_piece = captured_piece;

    pos->history.num_used_slots++;
}

static void position_hist_pop(struct position *const pos, struct move *mv, enum piece *pce_moved,
                              enum piece *captured_piece) {
    assert(validate_position_history(pos));
    assert(mv != NULL);
    assert(pce_moved != NULL);
    assert(captured_piece != NULL);

    pos->history.num_used_slots--;
    struct history_item *free_slot = &pos->history.items[pos->history.num_used_slots];

    __builtin_memcpy_inline(&pos->state, &free_slot->state, sizeof(struct game_state));

    *mv = free_slot->mv;
    *pce_moved = free_slot->pce_moved;
    *captured_piece = free_slot->captured_piece;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

static bool validate_position_history(const struct position *const pos) {
    const struct history_item *free_slot = &pos->history.items[pos->history.num_used_slots];

    if (free_slot < &pos->history.items[0]) {
        printf("Pos Hist free slot ptr is less than item[0], pos->history.num_used_slots=%d\n",
               pos->history.num_used_slots);
        return false;
    }

    if (free_slot > &pos->history.items[MAX_GAME_MOVES - 1]) {
        printf("Pos Hist free slot ptr is > item[MAX_GAME_MOVES - 1], pos->history.num_used_slots=%d\n",
               pos->history.num_used_slots);
        return false;
    }

    return true;
}
#pragma GCC diagnostic pop

static bool position_hist_compare(const struct position *const pos1, const struct position *const pos2) {

    if (pos1->history.num_used_slots != pos2->history.num_used_slots) {
        return false;
    }

    uint16_t num_slots = pos1->history.num_used_slots;

    for (int i = 0; i < num_slots; i++) {

        const struct history_item *hi1 = &pos1->history.items[i];
        const struct history_item *hi2 = &pos2->history.items[i];

        bool same_game_state = compare_game_states(&hi1->state, &hi2->state);
        if (same_game_state == false) {
            return false;
        }

        if (hi1->pce_moved != hi2->pce_moved) {
            return false;
        }

        if (hi1->captured_piece != hi2->captured_piece) {
            return false;
        }
    }
    return true;
}

static bool compare_game_states(const struct game_state *gs1, const struct game_state *gs2) {
    if (gs1->hashkey != gs2->hashkey) {
        return false;
    }

    if (gs1->side_to_move != gs2->side_to_move) {
        return false;
    }

    if (gs1->ply != gs2->ply) {
        return false;
    }
    if (gs1->history_ply != gs2->history_ply) {
        return false;
    }

    if (gs1->fifty_move_counter != gs2->fifty_move_counter) {
        return false;
    }

    if (gs1->en_passant_sq != gs2->en_passant_sq) {
        return false;
    }

    bool same_cast_perms = cast_compare_perms(gs1->castle_perm_container, gs2->castle_perm_container);
    if (same_cast_perms == false) {
        return false;
    }

    return true;
}

//
// functions to manipulate pieces and update hashes
//
static inline void pos_move_piece(struct position *const pos, const enum piece pce, const enum square from_sq,
                                  const enum square to_sq) {
    brd_move_piece(pos->brd, pce, from_sq, to_sq);
    pos->state.hashkey = hash_piece_update_move(pce, from_sq, to_sq, pos->state.hashkey);
}

static inline void pos_remove_piece(struct position *const pos, const enum piece pce, const enum square sq) {
    brd_remove_piece(pos->brd, pce, sq);
    pos->state.hashkey = hash_piece_update(pce, sq, pos->state.hashkey);
}

static inline void pos_add_piece(struct position *const pos, const enum piece pce, const enum square sq) {
    brd_add_piece(pos->brd, pce, sq);
    pos->state.hashkey = hash_piece_update(pce, sq, pos->state.hashkey);
}
