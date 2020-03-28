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

#include "perft_runner.h"
#include "perft_file_reader.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


//uint64_t do_perft(const uint8_t depth, struct position *pos);

int main(void) {

    struct perft_epd parsed = perft_load_file("perftsuite.epd");

    printf("count = %d\n", parsed.row_count);
}


// uint64_t do_perft(const uint8_t depth, struct position *pos){
//     uint64_t nodes = 0;
//     if (depth == 0){
//         return 1;
//     }



// }



// pub fn perft(depth: u8, position: &mut Position) -> u64 {
//     let mut nodes = 0;
//     if depth == 0 {
//         return 1;
//     }

//     let mut move_list = Vec::new();

//     move_gen::generate_moves(position, &mut move_list);

//     for mv in &move_list {
//         let move_legality = position.make_move(*mv);
//         if move_legality == MoveLegality::Legal {
//             nodes = nodes + perft(depth - 1, position);
//         }
//         position.take_move();
//     }

//     //println!("#Nodes: {}, Move List: {:?}", nodes, move_list);
//     return nodes;
// }






