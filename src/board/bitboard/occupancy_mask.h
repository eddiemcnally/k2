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


#pragma once

uint64_t occ_mask_get_black_pawn_capture_non_first_double_move ( const enum square sq );
uint64_t occ_mask_get_white_pawn_capture_non_first_double_move ( const enum square sq );
uint64_t occ_mask_get_knight ( const enum square sq );
uint64_t occ_mask_get_bishop ( const enum square sq );
uint64_t occ_mask_get_king ( const enum square sq );
uint64_t occ_mask_get_queen ( const enum square sq );
uint64_t occ_mask_get_rook ( const enum square sq );
uint64_t occ_mask_get_positive_diagonal ( const enum square sq );
uint64_t occ_mask_get_negative_diagonal ( const enum square sq );
uint64_t occ_mask_get_vertical ( const enum square sq );
uint64_t occ_mask_get_horizontal ( const enum square sq );

