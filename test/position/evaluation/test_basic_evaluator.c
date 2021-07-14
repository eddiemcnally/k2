/*  MIT License
 *
 *  Copyright (c) 2020 Eddie McNally
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

#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "test_basic_evaluator.h"
#include "basic_evaluator.h"
#include <cmocka.h>

void test_basic_evaluator_sample_white_position(void **state) {
    const char *FEN = "k7/8/1P3B2/P6P/3Q4/1N6/3K4/7R w - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    const struct board *brd = pos_get_board(pos);
    int32_t score = evaluate_position_basic(brd, WHITE);

    assert_true(score == 2365);

    // let fen = "k7/8/1P3B2/P6P/3Q4/1N6/3K4/7R w - - 0 1";
    //       let parsed_fen = fen::get_position(&fen);
    //       let zobrist_keys = ZobristKeys::new();
    //       let occ_masks = OccupancyMasks::new();

    //       let pos = Position::new(&zobrist_keys, &occ_masks, parsed_fen);

    //       let score = super::evaluate_board(pos.board(), Colour::White);
    //       assert_eq!(score, 2365);

    // Pawn = 100,
    // Knight = 320,
    // Bishop = 330,
    // Rook = 500,
    // Queen = 900,
    // King = 20000,

    // white material = 22350
    //  - 3x pawns      = 300
    //  - 1x knight     = 320
    //  - 1x rook       = 500
    //  - 1x bishop     = 330
    //  - 1x queen      = 900
    //  - 1x king       = 20000
    //
    // black material = 20000
    //  - 1x king       = 20000

    //
    // white piece positions = 35
    //  - Pawns: 5 + 10 + 5 = 20
    //  - Knight: 5         = 5
    //  - Queen: 5          = 5
    //  - King: 0           = 0
    //  - Bishop: 5         = 5
    //  - Rook: 0           = 0
    //
    // Black position pieces = 20
    //
    // expected score   = (22350 - 20000) + (35 - 20)
    //                  = 2365
}

void test_basic_evaluator_sample_black_position(void **state) {
    const char *FEN = "1k6/1pp3q1/5b2/1n6/7p/8/3K4/8 b - - 0 1";

    struct position *pos = pos_create();
    pos_initialise(FEN, pos);

    const struct board *brd = pos_get_board(pos);
    int32_t score = evaluate_position_basic(brd, BLACK);

    printf("****score %d\n", score);
    assert_int_equal(score, 1915);

    // let fen = "1k6/1pp3q1/5b2/1n6/7p/8/3K4/8 b - - 0 1";
    //        let parsed_fen = fen::get_position(&fen);
    //        let zobrist_keys = ZobristKeys::new();
    //        let occ_masks = OccupancyMasks::new();

    //        let pos = Position::new(&zobrist_keys, &occ_masks, parsed_fen);

    //        let score = super::evaluate_board(pos.board(), Colour::White);
    //        assert_eq!(score, -1915);

    // white material = 20000
    //  - 1x king       = 20000
    //
    // black material = 21850
    //  - 3x pawns      = 300
    //  - 1x knight     = 320
    //  - 1x bishop     = 330
    //  - 1x queen      = 900
    //  - 1x king       = 20000
    //
    // Black piece positions    = 65
    //  - Pawns: 10 + 10 + 5 = 25
    //  - Knight: 5          = 5
    //  - Queen: 0           = 0
    //  - King: 30           = 30
    //  - Bishop: 5          = 5
    //
    // White position pieces = 0
    //
    // expected score   = (20000 - 21850) + (0 - 60)
    //                  = 1915 (inverted for black)
}
