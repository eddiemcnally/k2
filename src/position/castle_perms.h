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

#include <stdint.h>
#include <stdbool.h>


typedef uint8_t cast_perm_t;

bool cast_perm_has_WK(const cast_perm_t cp);
bool cast_perm_has_WQ(const cast_perm_t cp);
bool cast_perm_has_BK(const cast_perm_t cp);
bool cast_perm_has_BQ(const cast_perm_t cp);
bool cast_perm_has_perms(const cast_perm_t cp);


void cast_perm_set_WK(cast_perm_t* cp, const bool enabled);
void cast_perm_set_WQ(cast_perm_t* cp, const bool enabled);
void cast_perm_set_BK(cast_perm_t* cp, const bool enabled);
void cast_perm_set_BQ(cast_perm_t* cp, const bool enabled);
void cast_perm_set_no_perms(cast_perm_t* cp);



