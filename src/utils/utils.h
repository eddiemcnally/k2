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

#pragma once

#include "k2_engine.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void print_stacktrace(void);
void print_stacktrace_and_exit(void);

double get_time_of_day_in_secs(void);
double get_elapsed_time_in_secs(double start_time);
uint64_t round_down_to_nearest_power_2(uint64_t n);
void prefetch(void *addr);

/**
 * @brief A non-debug mode runtime assert.
 * 
 */
#define REQUIRE(cond, text)                                                                                            \
    if (!(cond)) {                                                                                                     \
        fprintf(stderr, "FATAL: Error condition\n");                                                                   \
        fprintf(stderr, "\tFile     :    %s\n", __FILE__);                                                             \
        fprintf(stderr, "\tLine     :    %d\n", __LINE__);                                                             \
        fprintf(stderr, "\tFunction :    %s\n", __FUNCTION__);                                                         \
        fprintf(stderr, "\tText     :    %s\n", text);                                                                 \
        print_stacktrace();                                                                                            \
        exit(EXIT_FAILURE);                                                                                            \
    }
