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


#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "move_list.h"
#include "test_move_list.h"


void test_move_list_init ( void **state )
{
        struct move_list* mvl = mvl_allocate();

        uint16_t count = mvl_get_move_count ( mvl );
        assert_true ( count == 0 );

        mvl_deallocate ( mvl );
}


void test_move_list_bulk_add_moves ( void **state )
{
        const uint16_t max_moves = mvl_get_mvl_max_size() - 1;
        const uint16_t mv_offset = 1234;

        struct move_list* mvl = mvl_allocate();

        // add moves
        for ( int i = 0; i < max_moves; i++ ) {
                move_t mv = ( move_t ) ( mv_offset + i );
                mvl_add ( mvl, mv );
        }

        uint16_t count = mvl_get_move_count ( mvl );
        assert_true ( count == max_moves );

        for ( int i = 0; i < max_moves; i++ ) {
                move_t mv = mvl_get_move_at_offset(mvl, i);
                assert_true ( mv == ( move_t ) ( mv_offset + i ) );
        }

        mvl_deallocate ( mvl );
}

void test_move_list_contains_move ( void **state )
{
        const uint16_t num_moves = 250;
        struct move_list* mvl = mvl_allocate();

        // add moves
        for ( int i = 0; i < num_moves; i++ ) {
                move_t mv = ( move_t ) i;
                mvl_add ( mvl, mv );
        }

        // verify all are present
        for ( int i = 0; i < num_moves; i++ ) {
                move_t mv = ( move_t ) i;
                assert_true ( mvl_contains_move ( mvl, mv ) );
        }

        // verify a non-existant move is not in the list
        move_t other_mv = ( move_t ) ( num_moves + 100 );
        assert_false ( mvl_contains_move ( mvl, other_mv ) );

        mvl_deallocate ( mvl );
}


void test_move_list_reset_list ( void **state )
{
        const uint16_t num_moves = 30;
        struct move_list* mvl = mvl_allocate();

        // add moves
        for ( int i = 0; i < num_moves; i++ ) {
                move_t mv = ( move_t ) i;
                mvl_add ( mvl, mv );
        }

        assert_true ( num_moves == mvl_get_move_count ( mvl ) );

        mvl_reset ( mvl );
        assert_true ( 0 == mvl_get_move_count ( mvl ) );

        mvl_deallocate ( mvl );
}
