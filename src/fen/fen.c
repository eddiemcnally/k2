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

/*! @addtogroup FEN_Manipulation
 *
 * @ingroup FEN_Manipulation
 * @{
 * @details Provides functions to parse a FEN and extract the information into a set of structs and enums.
 *
 */

#include "fen.h"
#include "piece.h"
#include "position.h"
#include "square.h"
#include "utils.h"
#include <assert.h>

#define MAX_FEN 255

static void init_parsed_fen(struct parsed_fen *pf);
static void setup_piece_positions(struct parsed_fen *pf, const char *pieces);
static void setup_side_to_move(struct parsed_fen *pf, const char *side);
static void setup_castle_permissions(struct parsed_fen *pf, const char *perms);
static void setup_en_passant_sq(struct parsed_fen *pf, const char *en_pass);
static void setup_half_move_count(struct parsed_fen *pf, const char *half_move_cnt);
static void setup_full_move_count(struct parsed_fen *pf, const char *full_move_cnt);
static uint16_t convert_move_count(const char *str);
static void handle_rank(struct parsed_fen *pf, const enum rank rank, const char *pieces);
static bool validate_struct_init(const struct parsed_fen *pf);

// ==================================================================
//
// Data structs
//
// ==================================================================

struct piece_location {
    enum piece piece;
    bool is_occupied;
};

// used to check struct is populated when passed into public functions
static uint16_t STRUCT_INIT_KEY = 0xdead;

struct castle_perms {
    bool has_wq_cast_perm;
    bool has_wk_cast_perm;
    bool has_bq_cast_perm;
    bool has_bk_cast_perm;
};

struct parsed_fen {
    uint16_t struct_init_key;
    // state of each square
    struct piece_location pieces[NUM_SQUARES];
    uint16_t half_move_cnt;
    uint16_t full_move_cnt;
    enum colour side_to_move;
    struct castle_perms castle_permissions;
    bool is_en_pass_set;
    enum square en_pass_sq;
};

// there's only 1 instance of this!!
static struct parsed_fen decomposed_fen;

// ==================================================================
//
// public functions
//
// ==================================================================

/**
 * @brief       akes a FEN string, parses it and returns a ptr to a struct with the parsed data
 *
 * @param       fen_string      FEN-formatted string
 * @return      Pointer to struct parsed_fen
 */
struct parsed_fen *fen_parse(const char *fen_string) {
    char space_delim[] = " ";

    // local copy for tokenising
    char fen[MAX_FEN] = {0};

    memcpy(fen, fen_string, strlen(fen_string));

    // set up return value
    struct parsed_fen *retval = &decomposed_fen;
    init_parsed_fen(retval);

    // split the FEN string into separate fragments
    char *pieces = strtok(fen, space_delim);
    char *side = strtok(NULL, space_delim);
    char *cast_perms = strtok(NULL, space_delim);
    char *en_pass = strtok(NULL, space_delim);
    char *half_move_cnt = strtok(NULL, space_delim);
    char *full_move_cnt = strtok(NULL, space_delim);

    // parse the fragments and populate the struct
    setup_piece_positions(retval, pieces);
    setup_side_to_move(retval, side);
    setup_castle_permissions(retval, cast_perms);
    setup_en_passant_sq(retval, en_pass);
    setup_half_move_count(retval, half_move_cnt);
    setup_full_move_count(retval, full_move_cnt);

    return retval;
}

/**
 * @brief       Tries to extract the piece on the given square using the parsed_fen struct
 *
 * @param       pf      The struct containing the parsed FEN
 * @param       sq      The square
 * @param       pce     Pointer where returned piece will be saved
 * @return      true if piece found, false otherwise
 */
bool fen_try_get_piece_on_sq(const struct parsed_fen *pf, const enum square sq, enum piece *pce) {
    if (pf->pieces[sq].is_occupied == true) {
        *pce = pf->pieces[sq].piece;
        //printf("returning piece %c\n", get_label(*pce));
        return true;
    }
    return false;
}

bool fen_has_wk_castle_perms(const struct parsed_fen *pf) {
    return pf->castle_permissions.has_wk_cast_perm;
}
bool fen_has_wq_castle_perms(const struct parsed_fen *pf) {
    return pf->castle_permissions.has_wq_cast_perm;
}
bool fen_has_bk_castle_perms(const struct parsed_fen *pf) {
    return pf->castle_permissions.has_bk_cast_perm;
}
bool fen_has_bq_castle_perms(const struct parsed_fen *pf) {
    return pf->castle_permissions.has_bq_cast_perm;
}

/**
 * @brief       Given the struct, attempts to get the piece on the given square
 *
 * @param       pf              The FEN struct
 * @param       sq              The square being tested
 * @return      true if a piece was found, false otherwise
 */
bool fen_try_get_en_pass_sq(const struct parsed_fen *pf, enum square *sq) {
    assert(validate_struct_init(pf));

    if (pf->is_en_pass_set == true) {
        *sq = pf->en_pass_sq;
        return true;
    }
    return false;
}

/**
 * @brief       Examines the given struct and returns the side to move
 *
 * @param       pf  The struct to examine
 * @return      The side to move
 */
enum colour fen_get_side_to_move(const struct parsed_fen *pf) {
    assert(validate_struct_init(pf));

    return pf->side_to_move;
}

/**
 * @brief       Examines the given struct and returns the half move count
 *
 * @param       pf   The struct to examine
 * @return      The half move count
 */
uint16_t fen_get_half_move_cnt(const struct parsed_fen *pf) {
    assert(validate_struct_init(pf));

    return pf->half_move_cnt;
}

/**
 * @brief       Examines the given struct and returns the full move count
 *
 * @param       pf  The struct to examine
 * @return      The full move count
 */
uint16_t fen_get_full_move_cnt(const struct parsed_fen *pf) {
    assert(validate_struct_init(pf));

    return pf->full_move_cnt;
}

// ==================================================================
//
// private functions
//
// ==================================================================

static void init_parsed_fen(struct parsed_fen *pf) {
    memset(pf, 0, sizeof(struct parsed_fen));

    pf->struct_init_key = STRUCT_INIT_KEY;
    struct piece_location *plocs = pf->pieces;
    for (int i = 0; i < NUM_SQUARES; i++) {
        plocs->is_occupied = false;
        plocs++;
    }

    pf->side_to_move = WHITE;
    pf->is_en_pass_set = false;
}

static void setup_piece_positions(struct parsed_fen *pf, const char *pieces) {
    char rank_delim[] = "/";

    // copy for tokenising
    char tmp[MAX_FEN] = {0};

    memcpy(tmp, pieces, strlen(pieces));

    // split into ranks sections
    char *rank8 = strtok(tmp, rank_delim);

    handle_rank(pf, RANK_8, rank8);

    for (int r = RANK_7; r >= RANK_1; r--) {
        char *rank_str = strtok(NULL, rank_delim);
        handle_rank(pf, (enum rank)r, rank_str);
    }
}

static void handle_rank(struct parsed_fen *pf, const enum rank rank, const char *pieces) {
    enum file file = FILE_A;

    while (*pieces) {
        enum piece piece_to_add;
        bool piece_found = true;

        char c = *pieces;

        if (isdigit(c)) {
            //printf("found number %c\n", c);
            file += (uint8_t)((c) - '0');
            piece_found = false;
        } else {
            piece_to_add = pce_get_from_label(c);
        }

        if (piece_found == true) {
            //printf("found piece %c\n", c);
            enum square sq = sq_gen_from_rank_file((enum rank)rank, (enum file)file);

            //printf("adding piece %c to square %s\n", c, print_square(sq));

            pf->pieces[sq].is_occupied = true;
            pf->pieces[sq].piece = piece_to_add;
            file++;
        }

        pieces++;
    }
}

static void setup_side_to_move(struct parsed_fen *pf, const char *side) {
    if (*side == 'w') {
        pf->side_to_move = WHITE;
    } else {
        pf->side_to_move = BLACK;
    }
}

static void setup_castle_permissions(struct parsed_fen *pf, const char *perms) {
    // default to none
    pf->castle_permissions.has_wk_cast_perm = false;
    pf->castle_permissions.has_wq_cast_perm = false;
    pf->castle_permissions.has_bk_cast_perm = false;
    pf->castle_permissions.has_bq_cast_perm = false;

    if (*perms == '-') {
        return;
    }

    uint8_t len = (uint8_t)strlen(perms);

    for (int i = 0; i < len; i++) {
        switch (*perms) {
        case 'K':
            pf->castle_permissions.has_wk_cast_perm = true;
            break;
        case 'Q':
            pf->castle_permissions.has_wq_cast_perm = true;
            break;
        case 'k':
            pf->castle_permissions.has_bk_cast_perm = true;
            break;
        case 'q':
            pf->castle_permissions.has_bq_cast_perm = true;
            break;
        default:
            REQUIRE(false, "invalid Castle Permission character");
        }
        perms++;
    }
}

static void setup_en_passant_sq(struct parsed_fen *pf, const char *en_pass) {
    if (*en_pass != '-') {
        // en passant square present
        int file = en_pass[0] - 'a';
        int rank = en_pass[1] - '1';

        pf->is_en_pass_set = true;
        pf->en_pass_sq = sq_gen_from_rank_file((enum rank)rank, (enum file)file);
    } else {
        pf->is_en_pass_set = false;
    }
}

static void setup_half_move_count(struct parsed_fen *pf, const char *half_move_cnt) {
    uint16_t half_move = convert_move_count(half_move_cnt);
    pf->half_move_cnt = half_move;
}

static void setup_full_move_count(struct parsed_fen *pf, const char *full_move_cnt) {
    uint16_t full_move = convert_move_count(full_move_cnt);
    pf->full_move_cnt = full_move;
}

static uint16_t convert_move_count(const char *str) {
    int result = atoi(str);
    return (uint16_t)result;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static bool validate_struct_init(const struct parsed_fen *pf) {
    if (pf->struct_init_key != STRUCT_INIT_KEY) {
        return false;
    }
    return true;
}

#pragma GCC diagnostic pop
