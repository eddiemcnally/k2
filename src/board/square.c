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


#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "square.h"

// Temp storage when generatinga string representation of a square
#define MAX_SQUARE_TEXT 4
static char square_text[MAX_SQUARE_TEXT];


enum rank sq_get_rank ( const enum square sq )
{
        assert ( validate_square ( sq ) );

        return ( enum rank ) ( sq >> 3 );
}

enum file sq_get_file ( const enum square sq )
{
        assert ( validate_square ( sq ) );

        return ( enum file ) ( sq % 8 );
}

enum square sq_gen_from_rank_file ( const enum rank rank, const enum file file )
{
        assert ( validate_rank ( rank ) );
        assert ( validate_file ( file ) );

        return ( enum square ) ( ( rank << 3 ) + file );
}

char * print_square ( const enum square sq )
{
        assert ( validate_square ( sq ) );

        int file = sq_get_file ( sq );
        int rank = sq_get_rank ( sq );

        sprintf ( square_text, "%c%c", ( 'a' + file ), ( '1' + rank ) );

        return square_text;
}

bool validate_square ( const enum square sq )
{
        if ( sq >= a1 && sq <= h8 ) {
                return true;
        }
        return false;
}

bool validate_rank ( const enum rank rank )
{
        if ( rank >= RANK_1 && rank <= RANK_8 ) {
                return true;
        }
        return false;
}

bool validate_file ( const enum file file )
{
        if ( file  >= FILE_A && file <= FILE_H ) {
                return true;
        }
        return false;
}

// kate: indent-mode cstyle; indent-width 8; replace-tabs on; 
