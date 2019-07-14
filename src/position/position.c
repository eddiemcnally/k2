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

#include "position.h"
#include "board.h"
#include "castle_perms.h"
#include "fen.h"
#include "move.h"
#include <assert.h>

// key used to verify struct has been initialised
const static uint16_t STRUCT_INIT_KEY = 0xdead;

#define MAX_GAME_MOVES (1024)

// represents board state *before* the move was made
struct mv_state {
    struct move mv; // TODO: is this needed?
    uint8_t fifty_move_counter;
    enum square en_passant;
    bool en_pass_set;

    struct cast_perm_container castle_perm_container;
    // TODO add board hash
};

struct position {
    uint16_t struct_init_key;

    // current board representation
    struct board *brd;

    // the next side to move
    enum colour side_to_move;

    // keeping track of ply
    uint16_t ply;         // half-moves
    uint16_t history_ply; // full move count

    // state
    uint8_t fifty_move_counter;
    enum square en_passant;
    bool en_passant_set;

    // move history
    struct mv_state history[MAX_GAME_MOVES];

    struct cast_perm_container castle_perm_container;
};

static void init_pos_struct(struct position *pos);
static void populate_position_from_fen(struct position *pos,
                                       const struct parsed_fen *fen);
static void set_up_castle_permissions(struct position *pos,
                                      const struct parsed_fen *fen);
static bool validate_en_passant_pce_and_sq(const struct position *pos);
static void push_position(struct position *pos, const struct move move);
static struct move pop_position(struct position *pos);
static bool mv_state_compare(const struct mv_state *first,
                             const struct mv_state *second);
static enum square get_en_pass_sq(const enum colour side,
                                  const enum square from_sq);
static void do_capture_move(struct position *pos, const struct move mv,
                            const enum square from_sq, const enum square to_sq,
                            const struct piece pce_to_move);
static void make_castle_piece_moves(struct position *pos,
                                    const struct move castle_move);
static void make_en_passant_move(struct position *pos,
                                 const struct move en_pass_mv);

/**
 * @brief       Create and initialise an empty instance of the Position struct
 *
 * @return      An initialised Position struct
 */
struct position *pos_create() {
    struct position *retval =
        (struct position *)malloc(sizeof(struct position));
    init_pos_struct(retval);

    struct board *brd = brd_allocate();
    retval->brd = brd;

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
bool pos_try_get_en_pass_sq(const struct position *pos,
                            enum square *en_pass_sq) {
    if (pos->en_passant_set) {
        *en_pass_sq = pos->en_passant;
        return true;
    }
    return false;
}

/**
 * @brief               Sets the current Castle permissions
 *
 * @param pos           The position
 * @param perms         Castle permissions to set
 */
void pos_set_cast_perm(struct position *pos,
                       const struct cast_perm_container perms) {
    pos->castle_perm_container = perms;
}

/**
 * @brief               Validates the current position
 *
 * @param pos           The position
 * @param perms         true if position is valid/consistent, false otherwise
 */
bool validate_position(const struct position *pos) {
    assert(pos->struct_init_key == STRUCT_INIT_KEY);

    assert(validate_board(pos->brd));
    assert(validate_colour(pos->side_to_move));

    return true;
}

/* to do
 * =====
 * 
 * save current position (->history)
 *      current hash
 *      move being made
 *      current castle permissions
 *      en passant square
 *      current value of  "50-move rule" counter
 * incr history pointer
 * extract top/from squares, capture move
 * if capture move, 
 *      save capture piece (->history)
 *      remove piece from board
 * move piece from->to
 * update 50 move rule 
 *      increment counter
 *      reset if capture or pawn move
 *      if limit reached, declare draw.
 * handle promotion
 *      remove pawn (plus material)
 *      add promoted piece to board (increase material)
 * handle En passant capture
 *      remove pawn that was captured
 * Set En passant if double pawn move
 * update hashkey
 * adjust castle permissions based on move made
 * change side, incr ply and history ply        
 */

bool pos_try_make_move(struct position *pos, const struct move mv) {
    assert(validate_position(pos));

    push_position(pos, mv);

    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);
    struct piece pce_to_move;

    bool found = brd_try_get_piece_on_square(pos->brd, from_sq, &pce_to_move);
    assert(found == true);
    assert(validate_piece(pce_to_move));

    if (move_is_double_pawn(mv)) {
        pos->en_passant = get_en_pass_sq(pos->side_to_move, from_sq);
        pos->en_passant_set = true;

        brd_move_piece(pos->brd, pce_to_move, from_sq, to_sq);
    } else {
        pos->en_passant_set = false;
    }

    if (move_is_en_passant(mv)) {
        pos->en_passant_set = false;
        make_en_passant_move(pos, mv);
    }

    if (move_is_quiet(mv)) {
        if (move_is_promotion(mv)) {
            // quiet promotion
            struct piece pce_prom =
                move_decode_promotion_piece(mv, pos->side_to_move);
            brd_move_piece(pos->brd, pce_to_move, from_sq, to_sq);
            brd_remove_piece(pos->brd, pce_to_move, to_sq);
            brd_add_piece(pos->brd, pce_prom, to_sq);
        } else {
            // normal quiet move
            brd_move_piece(pos->brd, pce_to_move, from_sq, to_sq);
        }
    }

    if (move_is_capture(mv)) {
        do_capture_move(pos, mv, from_sq, to_sq, pce_to_move);
    }

    if (move_is_castle(mv)) {
        make_castle_piece_moves(pos, mv);
    }

    // swap sides
    pos->side_to_move = pce_swap_side(pos->side_to_move);

    return true;
}

struct move pos_take_move(struct position *pos) {
    assert(validate_position(pos));

    pop_position(pos);
    struct move mv = {.val = 0};
    return mv;
}

/**
 * @brief               Compares 2 positions for equivalency
 * @param first         The first position
 * @paran second        The second position
 * @return              True if the positions are the same, false otherwise
 */
bool pos_compare(const struct position *first, const struct position *second) {
    assert(validate_position(first));
    assert(validate_position(second));

    if (brd_compare(first->brd, second->brd) == false) {
        return false;
    }

    if (first->side_to_move != second->side_to_move) {
        return false;
    }

    if (first->ply != second->ply) {
        return false;
    }

    if (first->history_ply != second->history_ply) {
        return false;
    }

    if (first->fifty_move_counter != second->fifty_move_counter) {
        return false;
    }
    if (cast_compare_perms(first->castle_perm_container,
                           second->castle_perm_container) == false) {
        return false;
    }

    if (first->en_passant != second->en_passant) {
        return false;
    }
    if (first->en_passant_set != second->en_passant_set) {
        return false;
    }

    for (int i = 0; i < MAX_GAME_MOVES; i++) {
        const struct mv_state *mv1 = &first->history[i];
        const struct mv_state *mv2 = &second->history[i];

        if (mv_state_compare(mv1, mv2) == false) {
            return false;
        }
    }

    return true;
}

// ==================================================================
//
// private functions
//
// ==================================================================

static void init_pos_struct(struct position *pos) {
    memset(pos, 0, sizeof(struct position));
    pos->struct_init_key = STRUCT_INIT_KEY;

    pos->castle_perm_container = cast_perm_init();
}

static void do_capture_move(struct position *pos, const struct move mv,
                            const enum square from_sq, const enum square to_sq,
                            const struct piece pce_to_move) {
    assert(move_is_capture(mv));

    struct piece pce_capt;
    if (move_is_en_passant(mv)) {
        assert(validate_en_passant_pce_and_sq(pos));
        enum square en_pass_pce_sq;

        // flip the flag
        pos->en_passant_set = false;

        enum piece_role pt;
        if (pos->side_to_move == WHITE) {
            pt = PAWN;
            en_pass_pce_sq = sq_get_square_minus_1_rank(to_sq);
        } else {
            pt = PAWN;
            en_pass_pce_sq = sq_get_square_plus_1_rank(to_sq);
        }

        pce_capt = pce_create(pt, pos->side_to_move);
        brd_remove_piece(pos->brd, pce_capt, en_pass_pce_sq);
        brd_move_piece(pos->brd, pce_to_move, from_sq, to_sq);

    } else {
        bool found = brd_try_get_piece_on_square(pos->brd, to_sq, &pce_capt);
        assert(found == true);

        if (move_is_promotion(mv)) {
            struct piece pce_prom =
                move_decode_promotion_piece(mv, pos->side_to_move);
            brd_remove_piece(pos->brd, pce_to_move, from_sq);
            brd_add_piece(pos->brd, pce_prom, to_sq);
        } else {
            brd_remove_piece(pos->brd, pce_to_move, from_sq);
            brd_move_piece(pos->brd, pce_to_move, from_sq, to_sq);
        }
    }
}

static void make_castle_piece_moves(struct position *pos,
                                    const struct move castle_move) {
    assert(move_is_castle(castle_move));

    struct board *brd = pos_get_board(pos);

    const bool is_king_side = move_is_king_castle(castle_move);
    const bool is_queen_side = move_is_queen_castle(castle_move);
    const enum colour side = pos->side_to_move;

    const struct piece pce_wk = pce_create(KING, WHITE);
    const struct piece pce_wr = pce_create(ROOK, WHITE);
    const struct piece pce_bk = pce_create(KING, BLACK);
    const struct piece pce_br = pce_create(ROOK, BLACK);

    switch (side) {
    case WHITE:
        if (is_king_side) {
            brd_move_piece(brd, pce_wk, e1, g1);
            brd_move_piece(brd, pce_wr, h1, f1);
            cast_perm_set_permission(CP_WK, &pos->castle_perm_container, false);
        } else if (is_queen_side) {
            brd_move_piece(brd, pce_wk, e1, c1);
            brd_move_piece(brd, pce_wr, a1, d1);
            cast_perm_set_permission(CP_WQ, &pos->castle_perm_container, false);
        } else {
            assert(false);
        }
        break;
    case BLACK:
        if (is_king_side) {
            brd_move_piece(brd, pce_bk, e8, g8);
            brd_move_piece(brd, pce_br, h8, f8);
            cast_perm_set_permission(CP_BK, &pos->castle_perm_container, false);
        } else if (is_queen_side) {
            brd_move_piece(brd, pce_bk, e8, c8);
            brd_move_piece(brd, pce_br, a8, d8);
            cast_perm_set_permission(CP_BQ, &pos->castle_perm_container, false);
        } else {
            assert(false);
        }
        break;
    default:
        assert(false);
    }
}

static void make_en_passant_move(struct position *pos,
                                 const struct move en_pass_mv) {

    assert(move_is_en_passant(en_pass_mv));

    const enum square from_sq = move_decode_from_sq(en_pass_mv);
    const enum square to_sq = move_decode_to_sq(en_pass_mv);

    struct board *brd = pos_get_board(pos);

    struct piece pce_to_move;
    bool found = brd_try_get_piece_on_square(brd, from_sq, &pce_to_move);
    assert(found);

    enum square sq_with_piece;
    if (pos->side_to_move == WHITE) {
        sq_with_piece = sq_get_square_minus_1_rank(to_sq);
    } else if (pos->side_to_move == BLACK) {
        sq_with_piece = sq_get_square_plus_1_rank(to_sq);
    } else {
        assert(false);
    }

    assert(brd_is_sq_occupied(brd, sq_with_piece));

    struct piece pce_to_remove;
    found = brd_try_get_piece_on_square(brd, sq_with_piece, &pce_to_remove);

    brd_remove_piece(brd, pce_to_remove, sq_with_piece);
    brd_move_piece(brd, pce_to_move, from_sq, to_sq);
}

static void populate_position_from_fen(struct position *pos,
                                       const struct parsed_fen *fen) {
    pos->side_to_move = fen_get_side_to_move(fen);

    enum square en_pass;
    bool found_en_pass = fen_try_get_en_pass_sq(fen, &en_pass);
    if (found_en_pass) {
        pos->en_passant_set = true;
        pos->en_passant = en_pass;
    } else {
        pos->en_passant_set = false;
    }

    pos->fifty_move_counter = 0;
    pos->ply = fen_get_half_move_cnt(fen);
    pos->history_ply = fen_get_full_move_cnt(fen);
    set_up_castle_permissions(pos, fen);

    for (enum square sq = a1; sq <= h8; sq++) {
        struct piece pce;
        bool found_pce = fen_try_get_piece_on_sq(fen, sq, &pce);
        if (found_pce) {
            brd_add_piece(pos->brd, pce, sq);
        }
    }
}

static bool validate_en_passant_pce_and_sq(const struct position *pos) {
    assert(pos->en_passant_set == true);
    struct piece en_pass_pce;
    bool found =
        brd_try_get_piece_on_square(pos->brd, pos->en_passant, &en_pass_pce);
    assert(found == true);

    enum piece_role pt = pce_get_piece_type(en_pass_pce);
    assert(pt == PAWN);
    return true;
}
/*
void cast_perm_set_permission(const enum castle_permission cp, struct cast_perm_container *cp_cont,
                                          const bool state);
*/

static void set_up_castle_permissions(struct position *pos,
                                      const struct parsed_fen *fen) {

    struct cast_perm_container *cp = &pos->castle_perm_container;

    cast_perm_set_permission(CP_NONE, cp, true);

    if (fen_has_wk_castle_perms(fen)) {
        cast_perm_set_permission(CP_WK, cp, true);
    }
    if (fen_has_wq_castle_perms(fen)) {
        cast_perm_set_permission(CP_WQ, cp, true);
    }
    if (fen_has_bk_castle_perms(fen)) {
        cast_perm_set_permission(CP_BK, cp, true);
    }
    if (fen_has_bq_castle_perms(fen)) {
        cast_perm_set_permission(CP_BQ, cp, true);
    }
}

static enum square get_en_pass_sq(const enum colour side,
                                  const enum square from_sq) {

    enum square retval;
    if (side == WHITE) {
        retval = sq_get_square_plus_1_rank(from_sq);
    } else {
        retval = sq_get_square_minus_1_rank(from_sq);
    }
    assert(validate_en_pass_sq(retval));

    return retval;
}

static void push_position(struct position *pos, const struct move mv) {
    pos->ply++;

    struct mv_state *undo = &pos->history[pos->ply];

    undo->castle_perm_container = pos->castle_perm_container;
    undo->mv = mv;
    undo->en_pass_set = pos->en_passant_set;
    undo->en_passant = pos->en_passant;
    undo->fifty_move_counter = pos->fifty_move_counter;

    // backup the board
    brd_snaphot_make(pos->brd);
}

static struct move pop_position(struct position *pos) {

    struct mv_state *undo = &pos->history[pos->ply];

    pos->castle_perm_container = undo->castle_perm_container;
    struct move mv = undo->mv;
    pos->en_passant_set = undo->en_pass_set;
    pos->en_passant = undo->en_passant;
    pos->fifty_move_counter = undo->fifty_move_counter;

    pos->ply--;

    // revert to previous board
    brd_snaphot_extract(pos->brd);

    return mv;
}

static bool mv_state_compare(const struct mv_state *first,
                             const struct mv_state *second) {
    if (cast_compare_perms(first->castle_perm_container,
                           second->castle_perm_container) == false) {
        return false;
    }
    if (move_compare(first->mv, second->mv) == false) {
        return false;
    }
    if (first->en_pass_set != second->en_pass_set) {
        return false;
    }
    if (first->en_passant != second->en_passant) {
        return false;
    }
    if (first->fifty_move_counter != second->fifty_move_counter) {
        return false;
    }

    return true;
}
