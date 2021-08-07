
/*  MIT License
 *
 *  Copyright (c) 2021 Eddie McNally
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

#include "stats.h"
#include "utils.h"

#include <string.h>

struct stats_movegen {
    uint64_t num_moves;
    uint64_t num_illegal_moves;
    uint64_t num_quiet_moves;
    uint64_t num_capture_moves;
    uint64_t num_attacking_horiz_vert;
    uint64_t num_attacking_diagonal;
    uint64_t num_attacking_pce_knight;
    uint64_t num_attacking_pce_pawn;
    uint64_t num_attacking_pce_king;
};

struct engine_stats {
    struct stats_movegen move_gen;
    uint64_t init_flag;
};

#define INIT_KEY 0xdeadface

/**
 * @brief      Allocates a stats struct
 *
 * @return     Pointer to the allocated struct.
 */
struct engine_stats *stats_create(void) {
    struct engine_stats *retval = (struct engine_stats *)calloc(1, sizeof(struct engine_stats));
    retval->init_flag = INIT_KEY;
    return retval;
}

/**
 * @brief      Frees up mempory associated with given struct
 *
 * @param      stats  The statistics struct
 */
void stats_destroy(struct engine_stats *stats) {
    REQUIRE(stats->init_flag == INIT_KEY, "Stats struct isn't initialised");
    memset(stats, 0, sizeof(struct engine_stats));

    free(stats);
}

/**
 * @brief      Registers an illegal move with stats
 *
 * @param      stats  The statistics struct
 * @param[in]  pce    The attacking piece
 */
void stats_reg_illegal_move_attacking_pce(struct engine_stats *const stats, const enum attacking_type att_type) {
    REQUIRE(stats->init_flag == INIT_KEY, "Stats struct isn't initialised");

    stats->move_gen.num_illegal_moves++;

    switch (att_type) {
    case ATT_VERT_HORIZ:
        stats->move_gen.num_attacking_horiz_vert++;
        break;
    case ATT_DIAGONAL:
        stats->move_gen.num_attacking_diagonal++;
        break;
    case ATT_KNIGHT:
        stats->move_gen.num_attacking_pce_knight++;
        break;
    case ATT_PAWN:
        stats->move_gen.num_attacking_pce_pawn++;
        break;
    case ATT_KING:
        stats->move_gen.num_attacking_pce_king++;
        break;
    default:
        REQUIRE(false, "Stats attack type");
        break;
    }
}

/**
 * @brief      Registers a move with the stats 
 * 
 * @param      stats  The statistics struct
 * @param[in]  mv     The move being registered
 */
void stats_reg_move(struct engine_stats *const stats, const struct move mv) {
    REQUIRE(stats->init_flag == INIT_KEY, "Stats struct isn't initialised");

    stats->move_gen.num_moves++;

    if (move_is_quiet(mv)) {
        stats->move_gen.num_quiet_moves++;
    } else {
        stats->move_gen.num_capture_moves++;
    }
}

/**
 * @brief      Displays the stats
 *
 * @param      stats  The statistics struct
 */
void stats_display(struct engine_stats *const stats) {

    printf("***Stats ***\n");
    printf("   #moves.............................................: %llu\n", stats->move_gen.num_moves);
    printf("        #illegal......................................: %llu\n", stats->move_gen.num_illegal_moves);
    printf("        #quiet........................................: %llu\n", stats->move_gen.num_quiet_moves);
    printf("        #capture......................................: %llu\n", stats->move_gen.num_capture_moves);
    printf("\n");
    printf("        #attacking horizontal/vertical................: %llu\n", stats->move_gen.num_attacking_horiz_vert);
    printf("        #attacking diagonal...........................: %llu\n", stats->move_gen.num_attacking_diagonal);
    printf("        #attacking knight.............................: %llu\n", stats->move_gen.num_attacking_pce_knight);
    printf("        #attacking pawn...............................: %llu\n", stats->move_gen.num_attacking_pce_pawn);
    printf("        #attacking king...............................: %llu\n", stats->move_gen.num_attacking_pce_king);
}
