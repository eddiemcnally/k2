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
#pragma once

/*! @addtogroup Stats
 *
 * @ingroup Stats
 * @{
 * @details Provides support for engine statistics
 *
 */

#include "move.h"
#include "piece.h"

struct engine_stats;

enum attacking_type { ATT_VERT_HORIZ, ATT_DIAGONAL, ATT_KNIGHT, ATT_PAWN, ATT_KING };

struct engine_stats *stats_create(void);
void stats_destroy(struct engine_stats *stats);

void stats_reg_illegal_move_attacking_pce(struct engine_stats *const stats, const enum attacking_type att_type);
void stats_reg_move(struct engine_stats *const stats, const uint64_t mv);
void stats_reg_board_add_piece(struct engine_stats *const stats);
void stats_reg_board_remove_piece(struct engine_stats *const stats);

void stats_display(struct engine_stats *const stats);
