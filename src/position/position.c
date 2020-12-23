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
#include "position_hist.h"
#include "utils.h"
#include <assert.h>

// key used to verify struct has been initialised
const static uint16_t STRUCT_INIT_KEY = 0xdead;

// represents the current game position
struct position {
    uint16_t struct_init_key;

    // position hash
    uint64_t hashkey;

    // current board representation
    struct board *brd;

    // the next side to move
    enum colour side_to_move;

    // keeping track of ply
    uint16_t ply;         // half-moves
    uint16_t history_ply; // full move count

    uint8_t fifty_move_counter;

    enum square en_passant_sq;

    // active catle permissions
    struct cast_perm_container castle_perm_container;

    // position history
    struct position_hist *position_history;
};

static void init_pos_struct(struct position *pos);
static void populate_position_from_fen(struct position *pos, const struct parsed_fen *fen);
static void set_up_castle_permissions(struct position *pos, const struct parsed_fen *fen);
static enum square get_en_pass_sq(const enum colour side, const enum square from_sq);
static void do_capture_move(struct position *pos, const enum square from_sq, const enum square to_sq,
                            const enum piece pce_to_move);
static void make_king_side_castle_move(struct position *pos);
static void make_queen_side_castle_move(struct position *pos);
static void make_en_passant_move(struct position *pos, const struct move en_pass_mv, const enum square from_sq,
                                 const enum square to_sq);
static enum move_legality get_move_legal_status(const struct position *pos, const struct move mov);
static bool is_castle_move_legal(const struct position *pos, const struct move mov, const enum colour side_to_move,
                                 const enum colour attacking_side);
static void update_castle_perms(struct position *pos, const struct move mv, const enum piece pce_being_moved);
static void pos_move_piece(struct position *pos, const enum piece pce, const enum square from_sq,
                           const enum square to_sq);
static void pos_remove_piece(struct position *pos, const enum piece pce, const enum square sq);
static void pos_add_piece(struct position *pos, const enum piece pce, const enum square sq);
static void pos_update_castle_perm(struct position *pos, const enum castle_permission perm, const bool perm_state);
static void swap_side(struct position *pos);
static bool pos_is_en_passant_active(const struct position *pos);
static void do_promotion_quiet(struct position *pos, const enum piece pce_to_move, const enum square from_sq,
                               const enum square to_sq, const enum piece target_promotion_pce);
static void do_promotion_capture(struct position *pos, const enum piece pce_to_move, const enum square from_sq,
                                 const enum square to_sq, const enum piece target_promotion_pce);

// set up bitboards for all squares that need to be checked when
// testing castle move validity
const uint64_t WK_CAST_BB = (uint64_t)((0x01L << e1) | (0x01L << f1) | (0x01L << g1));
const uint64_t BK_CAST_BB = (uint64_t)((0x01L << e8) | (0x01L << f8) | (0x01L << g8));
const uint64_t WQ_CAST_BB = (uint64_t)((0x01L << c1) | (0x01L << d1) | (0x01L << e1));
const uint64_t BQ_CAST_BB = (uint64_t)((0x01L << c8) | (0x01L << d8) | (0x01L << e8));

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
void pos_initialise(const char *fen, struct position *pos) {
    struct parsed_fen *parsed_fen = fen_parse(fen);
    populate_position_from_fen(pos, parsed_fen);
}

/**
 * @brief       Cleans up the Position and frees up any memory
 *
 * @param pos   The position to destroy
 */
void pos_destroy(struct position *pos) {
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
struct board *pos_get_board(const struct position *pos) {
    assert(validate_position(pos));
    return pos->brd;
}

uint16_t pos_get_ply(const struct position *pos) {
    return pos->ply;
}

/**
 * @brief       Gets the side to move from the position struct
 *
 * @param pos   The position
 * @return      Side to move
 */
enum colour pos_get_side_to_move(const struct position *pos) {
    assert(validate_position(pos));
    return pos->side_to_move;
}

/**
 * @brief       Returns the current Castle Permissions
 *
 * @param pos   The position
 * @return      The castle permissions available
 */
struct cast_perm_container pos_get_cast_perm(const struct position *pos) {
    return pos->castle_perm_container;
}

/**
 * @brief               Tries to return the en passant square
 *
 * @param pos           The position
 * @param en_pass_sq
 * @return              true if valid/active en passent square, false otherwise
 */
enum square pos_get_en_pass_sq(const struct position *pos) {
    if (pos_is_en_passant_active(pos)) {
        return pos->en_passant_sq;        
    }
    return NO_SQUARE;
}

/**
 * @brief               Sets the current Castle permissions
 *
 * @param pos           The position
 * @param perms         Castle permissions to set
 */
void pos_set_cast_perm(struct position *pos, const struct cast_perm_container perms) {
    pos->castle_perm_container = perms;
}

/**
 * @brief               Validates the current position
 *
 * @param pos           The position
 * @param perms         true if position is valid/consistent, false otherwise
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
bool validate_position(const struct position *pos) {
    assert(pos->struct_init_key == STRUCT_INIT_KEY);

    assert(validate_board(pos->brd));
    assert(validate_colour(pos->side_to_move));

    return true;
}
#pragma GCC diagnostic pop

enum move_legality pos_make_move(struct position *pos, const struct move mv) {
    assert(validate_position(pos));

    position_hist_push(pos->position_history, mv, pos->fifty_move_counter, pos->en_passant_sq, pos->hashkey,
                       pos->castle_perm_container, pos->brd);

    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);

    enum piece pce_to_move;
    bool found = brd_try_get_piece_on_square(pos->brd, from_sq, &pce_to_move);
    REQUIRE(found, "no piece found on square");

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
        pos->en_passant_sq = get_en_pass_sq(pos->side_to_move, from_sq);
        pos_move_piece(pos, pce_to_move, from_sq, to_sq);
        break;
    case MV_TYPE_EN_PASS:
        pos->en_passant_sq = NO_SQUARE;
        make_en_passant_move(pos, mv, from_sq, to_sq);
        break;
    case MV_TYPE_QUEEN_CASTLE:
        make_queen_side_castle_move(pos);
        break;
    case MV_TYPE_KING_CASTLE:
        make_king_side_castle_move(pos);
        break;
    case MV_TYPE_PROMOTE_BISHOP: {
        const enum piece pce_prom = pos->side_to_move == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
        do_promotion_quiet(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_KNIGHT: {
        const enum piece pce_prom = pos->side_to_move == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
        do_promotion_quiet(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_QUEEN: {
        const enum piece pce_prom = pos->side_to_move == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
        do_promotion_quiet(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_ROOK: {
        const enum piece pce_prom = pos->side_to_move == WHITE ? WHITE_ROOK : BLACK_ROOK;
        do_promotion_quiet(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_BISHOP_CAPTURE: {
        const enum piece pce_prom = pos->side_to_move == WHITE ? WHITE_BISHOP : BLACK_BISHOP;
        do_promotion_capture(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_KNIGHT_CAPTURE: {
        const enum piece pce_prom = pos->side_to_move == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT;
        do_promotion_capture(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_QUEEN_CAPTURE: {
        const enum piece pce_prom = pos->side_to_move == WHITE ? WHITE_QUEEN : BLACK_QUEEN;
        do_promotion_capture(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    case MV_TYPE_PROMOTE_ROOK_CAPTURE: {
        const enum piece pce_prom = pos->side_to_move == WHITE ? WHITE_ROOK : BLACK_ROOK;
        do_promotion_capture(pos, pce_to_move, from_sq, to_sq, pce_prom);
    } break;
    default:
        REQUIRE(false, "Invalid move type");
    }

    // some cleanup
    const enum move_legality legality = get_move_legal_status(pos, mv);
    if (move_is_double_pawn(mv) == false) {
        pos->en_passant_sq = NO_SQUARE;
    }
    // clean out any castle permissions
    update_castle_perms(pos, mv, pce_to_move);

    swap_side(pos);

    return legality;
}

struct move pos_take_move(struct position *pos) {
    assert(validate_position(pos));

    //printf("*** TAKE MOVE \n");

    struct move mv;
    position_hist_pop(pos->position_history, &mv, &pos->fifty_move_counter, &pos->en_passant_sq, &pos->hashkey,
                      &pos->castle_perm_container, pos->brd);

    swap_side(pos);

    return mv;
}

static void do_promotion_quiet(struct position *pos, const enum piece pce_to_move, const enum square from_sq,
                               const enum square to_sq, const enum piece target_promotion_pce) {
    pos_remove_piece(pos, pce_to_move, from_sq);
    pos_add_piece(pos, target_promotion_pce, to_sq);
}

static void do_promotion_capture(struct position *pos, const enum piece pce_to_move, const enum square from_sq,
                                 const enum square to_sq, const enum piece target_promotion_pce) {

    enum piece pce_being_captured;
    bool piece_found = brd_try_get_piece_on_square(pos->brd, to_sq, &pce_being_captured);
    REQUIRE(piece_found, "no piece found for promotion");
    pos_remove_piece(pos, pce_being_captured, to_sq);

    pos_remove_piece(pos, pce_to_move, from_sq);
    pos_add_piece(pos, target_promotion_pce, to_sq);
}

static void swap_side(struct position *pos) {
    pos->side_to_move = pce_swap_side(pos->side_to_move);
    pos->hashkey = hash_side_update(pos->hashkey);
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

    if (first->side_to_move != second->side_to_move) {
        printf("pos_compare: side to move is different\n");
        return false;
    }

    if (first->ply != second->ply) {
        printf("pos_compare: ply is different\n");
        return false;
    }

    if (first->history_ply != second->history_ply) {
        printf("pos_compare: history_ply is different\n");
        return false;
    }

    if (first->fifty_move_counter != second->fifty_move_counter) {
        printf("pos_compare: 50 move counter is different\n");
        return false;
    }
    if (cast_compare_perms(first->castle_perm_container, second->castle_perm_container) == false) {
        printf("pos_compare: castle permissions are different\n");
        return false;
    }

    if (first->en_passant_sq != second->en_passant_sq) {
        printf("pos_compare: en passant state is different\n");
        return false;
    }

    bool same_hist = position_hist_compare(first->position_history, second->position_history);
    if (same_hist == false) {
        printf("pos_compare: position history is different\n");
        return false;
    }

    return true;
}

uint64_t pos_get_hash(const struct position *pos) {
    return pos->hashkey;
}

// ==================================================================
//
// private functions
//
// ==================================================================

static inline bool pos_is_en_passant_active(const struct position *pos) {
    return pos->en_passant_sq != NO_SQUARE;
}

static void init_pos_struct(struct position *pos) {
    memset(pos, 0, sizeof(struct position));
    pos->struct_init_key = STRUCT_INIT_KEY;

    pos->castle_perm_container = cast_perm_init();

    pos->position_history = position_hist_init();
}

static void do_capture_move(struct position *pos, const enum square from_sq, const enum square to_sq,
                            const enum piece pce_to_move) {
    enum piece pce_capt;
    bool found = brd_try_get_piece_on_square(pos->brd, to_sq, &pce_capt);
    REQUIRE(found, "Piece not found");

    pos_remove_piece(pos, pce_capt, to_sq);
    pos_move_piece(pos, pce_to_move, from_sq, to_sq);
}

static void update_castle_perms(struct position *pos, const struct move mv, const enum piece pce_being_moved) {
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

static enum move_legality get_move_legal_status(const struct position *pos, const struct move mov) {
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

static bool is_castle_move_legal(const struct position *pos, const struct move mov, const enum colour side_to_move,
                                 const enum colour attacking_side) {
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
        cast_bb = bb_clear_square(cast_bb, sq);
        if (att_chk_is_sq_attacked(pos->brd, sq, attacking_side)) {
            return false;
        }
    }
    return true;
}

static void make_king_side_castle_move(struct position *pos) {
    const enum colour side = pos->side_to_move;

    const enum piece pce_wk = WHITE_KING;
    const enum piece pce_wr = WHITE_ROOK;
    const enum piece pce_bk = BLACK_KING;
    const enum piece pce_br = BLACK_ROOK;

    switch (side) {
    case WHITE:
        pos_move_piece(pos, pce_wk, e1, g1);
        pos_move_piece(pos, pce_wr, h1, f1);

        pos_update_castle_perm(pos, CASTLE_PERM_WK, false);
        pos_update_castle_perm(pos, CASTLE_PERM_WQ, false);
        break;
    case BLACK:
        pos_move_piece(pos, pce_bk, e8, g8);
        pos_move_piece(pos, pce_br, h8, f8);

        pos_update_castle_perm(pos, CASTLE_PERM_BK, false);
        pos_update_castle_perm(pos, CASTLE_PERM_BQ, false);
        break;
    default:
        REQUIRE(false, "Unexpected condition");
    }
}

static void make_queen_side_castle_move(struct position *pos) {
    const enum colour side = pos->side_to_move;

    const enum piece pce_wk = WHITE_KING;
    const enum piece pce_wr = WHITE_ROOK;
    const enum piece pce_bk = BLACK_KING;
    const enum piece pce_br = BLACK_ROOK;

    switch (side) {
    case WHITE:
        pos_move_piece(pos, pce_wk, e1, c1);
        pos_move_piece(pos, pce_wr, a1, d1);
        pos_update_castle_perm(pos, CASTLE_PERM_WK, false);
        pos_update_castle_perm(pos, CASTLE_PERM_WQ, false);
        break;
    case BLACK:
        pos_move_piece(pos, pce_bk, e8, c8);
        pos_move_piece(pos, pce_br, a8, d8);
        pos_update_castle_perm(pos, CASTLE_PERM_BK, false);
        pos_update_castle_perm(pos, CASTLE_PERM_BQ, false);
        break;
    default:
        REQUIRE(false, "Unexpected condition");
    }
}

static void make_en_passant_move(struct position *pos, const struct move en_pass_mv, const enum square from_sq,
                                 const enum square to_sq) {

    REQUIRE(move_is_en_passant(en_pass_mv), "Move isn't en passant");

    enum piece pce_to_move;
    bool found = brd_try_get_piece_on_square(pos->brd, from_sq, &pce_to_move);
    REQUIRE(found, "No piece found");

    enum square sq_with_piece;
    if (pos->side_to_move == WHITE) {
        sq_with_piece = sq_get_square_minus_1_rank(to_sq);
    } else if (pos->side_to_move == BLACK) {
        sq_with_piece = sq_get_square_plus_1_rank(to_sq);
    } else {
        REQUIRE(false, "invalid side to move\n");
    }

    REQUIRE(brd_is_sq_occupied(pos->brd, sq_with_piece), "square isn't occupied");

    enum piece pce_to_remove;
    found = brd_try_get_piece_on_square(pos->brd, sq_with_piece, &pce_to_remove);
    REQUIRE(found, "piece not found on square \n");

    pos_remove_piece(pos, pce_to_remove, sq_with_piece);
    pos_move_piece(pos, pce_to_move, from_sq, to_sq);
    pos->hashkey = hash_en_passant(to_sq, pos->hashkey);
}

static void populate_position_from_fen(struct position *pos, const struct parsed_fen *fen) {
    pos->side_to_move = fen_get_side_to_move(fen);

    enum square en_pass;
    bool found_en_pass = fen_try_get_en_pass_sq(fen, &en_pass);
    if (found_en_pass) {
        pos->en_passant_sq = en_pass;
    } else {
        pos->en_passant_sq = NO_SQUARE;
    }

    pos->fifty_move_counter = 0;
    pos->ply = fen_get_half_move_cnt(fen);
    pos->history_ply = fen_get_full_move_cnt(fen);
    set_up_castle_permissions(pos, fen);

    for (enum square sq = a1; sq <= h8; sq++) {
        enum piece pce;
        bool found_pce = fen_try_get_piece_on_sq(fen, sq, &pce);
        if (found_pce) {
            pos_add_piece(pos, pce, sq);
        }
    }
}

static void set_up_castle_permissions(struct position *pos, const struct parsed_fen *fen) {

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

static void pos_update_castle_perm(struct position *pos, const enum castle_permission perm, const bool perm_state) {
    cast_perm_set_permission(perm, &pos->castle_perm_container, perm_state);
    pos->hashkey = hash_castle_perm(perm, pos->hashkey);
}

static enum square get_en_pass_sq(const enum colour side, const enum square from_sq) {

    enum square retval;
    if (side == WHITE) {
        retval = sq_get_square_plus_1_rank(from_sq);
    } else {
        retval = sq_get_square_minus_1_rank(from_sq);
    }
    assert(validate_en_pass_sq(retval));

    return retval;
}

//
// functions to manipulate pieces and update hashes
//
static inline void pos_move_piece(struct position *pos, const enum piece pce, const enum square from_sq,
                                  const enum square to_sq) {
    brd_move_piece(pos->brd, pce, from_sq, to_sq);
    pos->hashkey = hash_piece_update_move(pce, from_sq, to_sq, pos->hashkey);
}

static inline void pos_remove_piece(struct position *pos, const enum piece pce, const enum square sq) {
    brd_remove_piece(pos->brd, pce, sq);
    pos->hashkey = hash_piece_update(pce, sq, pos->hashkey);
}

static inline void pos_add_piece(struct position *pos, const enum piece pce, const enum square sq) {
    brd_add_piece(pos->brd, pce, sq);
    pos->hashkey = hash_piece_update(pce, sq, pos->hashkey);
}
