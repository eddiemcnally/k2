/*  MIT License
 *
 *  Copyright (c) 2017 Eddie McNally
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
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

    struct cast_perm castle_perm;
    // TODO add board hash
};

struct position {
    uint16_t struct_init_key;

    // current board representation
    struct board* brd;

    // the next side to move
    enum colour side_to_move;

    // keeping track of ply
    uint16_t ply; // half-moves
    uint16_t history_ply; // full move count

    // state
    uint8_t fifty_move_counter;
    enum square en_passant;
    bool en_passant_set;

    // move history
    struct mv_state history[MAX_GAME_MOVES];

    struct cast_perm castle_perm;
};

static void init_pos_struct(struct position* pos);
static void populate_position_from_fen(struct position* pos, const struct parsed_fen* fen);
static void set_up_castle_permissions(struct position* pos, const struct parsed_fen* fen);
static bool validate_en_passant_pce_and_sq(const struct position* pos);
static void push_position(struct position* pos, const struct move move);
static struct move pop_position(struct position* pos);
static bool mv_state_compare(const struct mv_state* first, const struct mv_state* second);
static enum square get_en_pass_sq(const enum colour side, const enum square from_sq);
static void do_capture_move(struct position* pos, const struct move mv, const enum square from_sq, const enum square to_sq, const enum piece pce_to_move);

static void make_castle_piece_moves(struct position* pos, const struct move castle_move);
static void make_king_side_castle_white_move(struct board* brd);
static void make_king_side_castle_black_move(struct board* brd);
static void make_queen_side_castle_white_move(struct board* brd);
static void make_queen_side_castle_black_move(struct board* brd);

/**
 * @brief       Create and initialise an empty instance of the Position struct
 *
 * @return      An initialised Position struct
 */
struct position* pos_create()
{
    struct position* retval = (struct position*)malloc(sizeof(struct position));
    init_pos_struct(retval);

    struct board* brd = brd_allocate();
    retval->brd = brd;

    return retval;
}

/**
 * @brief       Sets up the position using the goven FEN
 *
 * @param fen   The FEN string
 * @param pos   The position struct
 */
void pos_initialise(const char* fen, struct position* pos)
{
    struct parsed_fen* parsed_fen = fen_parse(fen);

    populate_position_from_fen(pos, parsed_fen);
}

/**
 * @brief       Cleans up the Position and frees up any memory
 *
 * @param pos   The position to destroy
 */
void pos_destroy(struct position* pos)
{
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
struct board* pos_get_board(const struct position* pos)
{
    assert(validate_position(pos));
    return pos->brd;
}

/**
 * @brief       Gets the side to move from the position struct
 *
 * @param pos   The position
 * @return      Side to move
 */
enum colour pos_get_side_to_move(const struct position* pos)
{
    assert(validate_position(pos));
    return pos->side_to_move;
}

/**
 * @brief       Returns the current Castle Permissions
 *
 * @param pos   The position
 * @return      The castle permissions available
 */
struct cast_perm pos_get_cast_perm(const struct position* pos)
{
    return pos->castle_perm;
}

/**
 * @brief               Tries to return the en passant square
 *
 * @param pos           The position
 * @param en_pass_sq
 * @return              true if valid/active en passent square, false otherwise
 */
bool pos_try_get_en_pass_sq(const struct position* pos, enum square* en_pass_sq)
{
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
void pos_set_cast_perm(struct position* pos, const struct cast_perm perms)
{
    pos->castle_perm = perms;
}

/**
 * @brief               Validates the current position
 *
 * @param pos           The position
 * @param perms         true if position is valid/consistent, false otherwise
 */
bool validate_position(const struct position* pos)
{
    assert(pos->struct_init_key == STRUCT_INIT_KEY);

    assert(validate_board(pos->brd));
    assert(validate_colour(pos->side_to_move));

    return true;
}

bool pos_try_make_move(struct position* pos, const struct move mv)
{
    assert(validate_position(pos));

    push_position(pos, mv);

    const enum square from_sq = move_decode_from_sq(mv);
    const enum square to_sq = move_decode_to_sq(mv);
    enum piece pce_to_move;

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

    if (move_is_quiet(mv)) {
        if (move_is_promotion(mv)) {
            // quiet promotion
            enum piece pce_prom = move_decode_promotion_piece(mv, pos->side_to_move);
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

struct move pos_take_move(struct position* pos)
{
    assert(validate_position(pos));

    struct move mv = {.val = 0 };
    return mv;
}

/**
 * @brief               Compares 2 positions for equivalency
 * @param first         The first position
 * @paran second        The second position
 * @return              True if the positions are the same, false otherwise
 */
bool pos_compare(const struct position* first, const struct position* second)
{
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
    if (cast_compare_perms(first->castle_perm, second->castle_perm) == false) {
        return false;
    }

    if (first->en_passant != second->en_passant) {
        return false;
    }
    if (first->en_passant_set != second->en_passant_set) {
        return false;
    }

    for (int i = 0; i < MAX_GAME_MOVES; i++) {
        const struct mv_state* mv1 = &first->history[i];
        const struct mv_state* mv2 = &second->history[i];

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

static void init_pos_struct(struct position* pos)
{
    memset(pos, 0, sizeof(struct position));
    pos->struct_init_key = STRUCT_INIT_KEY;
}

static void do_capture_move(struct position* pos, const struct move mv,
    const enum square from_sq, const enum square to_sq,
    const enum piece pce_to_move)
{
    assert(move_is_capture(mv));

    enum piece pce_capt;
    if (move_is_en_passant(mv)) {
        assert(validate_en_passant_pce_and_sq(pos));
        enum square en_pass_pce_sq;

        // flip the flag
        pos->en_passant_set = false;

        if (pos->side_to_move == WHITE) {
            pce_capt = BPAWN;
            en_pass_pce_sq = sq_get_square_minus_1_rank(to_sq);
        } else {
            pce_capt = WPAWN;
            en_pass_pce_sq = sq_get_square_plus_1_rank(to_sq);
        }
        brd_remove_piece(pos->brd, pce_capt, en_pass_pce_sq);
        brd_move_piece(pos->brd, pce_to_move, from_sq, to_sq);

    } else {
        bool found = brd_try_get_piece_on_square(pos->brd, to_sq, &pce_capt);
        assert(found == true);

        if (move_is_promotion(mv)) {
            enum piece pce_prom = move_decode_promotion_piece(mv, pos->side_to_move);
            brd_remove_piece(pos->brd, pce_to_move, from_sq);
            brd_add_piece(pos->brd, pce_prom, to_sq);
        } else {
            brd_remove_piece(pos->brd, pce_to_move, from_sq);
            brd_move_piece(pos->brd, pce_to_move, from_sq, to_sq);
        }
    }
}

static void make_castle_piece_moves(struct position* pos, const struct move castle_move)
{

    assert(move_is_castle(castle_move));

    struct board* brd = pos_get_board(pos);
    if (move_is_king_castle(castle_move)) {
        switch (pos->side_to_move) {
        case WHITE:
            make_king_side_castle_white_move(brd);
            cast_perm_set_WK(&pos->castle_perm, false);
            break;
        case BLACK:
            make_king_side_castle_black_move(brd);
            cast_perm_set_BK(&pos->castle_perm, false);
            break;
        default:
            assert(false);
        }
    } else if (move_is_queen_castle) {
        switch (pos->side_to_move) {
        case WHITE:
            make_queen_side_castle_white_move(brd);
            cast_perm_set_WQ(&pos->castle_perm, false);
            break;
        case BLACK:
            make_queen_side_castle_black_move(brd);
            cast_perm_set_BQ(&pos->castle_perm, false);
            break;
        default:
            assert(false);
        }
    }
}

static void make_king_side_castle_white_move(struct board* brd)
{

    const enum square WK_KING_FROM_SQ = e1;
    const enum square WK_KING_TO_SQ = g1;
    const enum square WK_ROOK_FROM_SQ = h1;
    const enum square WK_ROOK_TO_SQ = f1;

    brd_move_piece(brd, WKING, WK_KING_FROM_SQ, WK_KING_TO_SQ);
    brd_move_piece(brd, WROOK, WK_ROOK_FROM_SQ, WK_ROOK_TO_SQ);
}

static void make_king_side_castle_black_move(struct board* brd)
{

    const enum square BK_KING_FROM_SQ = e8;
    const enum square BK_KING_TO_SQ = g8;
    const enum square BK_ROOK_FROM_SQ = h8;
    const enum square BK_ROOK_TO_SQ = f8;

    brd_move_piece(brd, WKING, BK_KING_FROM_SQ, BK_KING_TO_SQ);
    brd_move_piece(brd, WROOK, BK_ROOK_FROM_SQ, BK_ROOK_TO_SQ);
}

static void make_queen_side_castle_white_move(struct board* brd)
{

    const enum square WK_KING_FROM_SQ = e1;
    const enum square WK_KING_TO_SQ = c1;
    const enum square WK_ROOK_FROM_SQ = a1;
    const enum square WK_ROOK_TO_SQ = d1;

    brd_move_piece(brd, WKING, WK_KING_FROM_SQ, WK_KING_TO_SQ);
    brd_move_piece(brd, WROOK, WK_ROOK_FROM_SQ, WK_ROOK_TO_SQ);
}

static void make_queen_side_castle_black_move(struct board* brd)
{

    const enum square BK_KING_FROM_SQ = e8;
    const enum square BK_KING_TO_SQ = c8;
    const enum square BK_ROOK_FROM_SQ = a8;
    const enum square BK_ROOK_TO_SQ = d8;

    brd_move_piece(brd, WKING, BK_KING_FROM_SQ, BK_KING_TO_SQ);
    brd_move_piece(brd, WROOK, BK_ROOK_FROM_SQ, BK_ROOK_TO_SQ);
}

static void populate_position_from_fen(struct position* pos, const struct parsed_fen* fen)
{
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
        enum piece pce;
        bool found_pce = fen_try_get_piece_on_sq(fen, sq, &pce);
        if (found_pce) {
            brd_add_piece(pos->brd, pce, sq);
        }
    }
}

static bool validate_en_passant_pce_and_sq(const struct position* pos)
{
    assert(pos->en_passant_set == true);
    enum piece en_pass_pce;
    bool found = brd_try_get_piece_on_square(pos->brd, pos->en_passant, &en_pass_pce);
    assert(found == true);
    assert(en_pass_pce == WPAWN || en_pass_pce == BPAWN);
    return true;
}

static void set_up_castle_permissions(struct position* pos, const struct parsed_fen* fen)
{
    struct cast_perm cp;
    // default to no castle permissions
    cast_perm_set_no_perms(&cp);

    if (fen_has_wk_castle_perms(fen)) {
        cast_perm_set_WK(&cp, true);
    }
    if (fen_has_wq_castle_perms(fen)) {
        cast_perm_set_WQ(&cp, true);
    }
    if (fen_has_bk_castle_perms(fen)) {
        cast_perm_set_BK(&cp, true);
    }
    if (fen_has_bq_castle_perms(fen)) {
        cast_perm_set_BQ(&cp, true);
    }

    pos_set_cast_perm(pos, cp);
}

static enum square get_en_pass_sq(const enum colour side, const enum square from_sq)
{

    enum square retval;
    if (side == WHITE) {
        retval = sq_get_square_plus_1_rank(from_sq);
    } else {
        retval = sq_get_square_minus_1_rank(from_sq);
    }
    assert(validate_en_pass_sq(retval));

    return retval;
}

static void push_position(struct position* pos, const struct move mv)
{
    pos->ply++;

    struct mv_state* undo = &pos->history[pos->ply];

    undo->castle_perm = pos->castle_perm;
    undo->mv = mv;
    undo->en_pass_set = pos->en_passant_set;
    undo->en_passant = pos->en_passant;
    undo->fifty_move_counter = pos->fifty_move_counter;

    // backup the board
    brd_snaphot_make(pos->brd);
}

static struct move pop_position(struct position* pos)
{

    struct mv_state* undo = &pos->history[pos->ply];

    pos->castle_perm = undo->castle_perm;
    struct move mv = undo->mv;
    pos->en_passant_set = undo->en_pass_set;
    pos->en_passant = undo->en_passant;
    pos->fifty_move_counter = undo->fifty_move_counter;

    pos->ply--;

    // revert to previous board
    brd_snaphot_extract(pos->brd);

    return mv;
}

static bool mv_state_compare(const struct mv_state* first, const struct mv_state* second)
{
    if (cast_compare_perms(first->castle_perm, second->castle_perm) == false) {
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
