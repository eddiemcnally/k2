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

/*! @addtogroup Perft
 *
 * @ingroup Perft
 * @{
 * @details Perft file-reader
 *
 */

#include "perft_file_reader.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct epd_row parse_row(char *row);
static uint64_t parse_depth_details(char *s);

static const char FIELD_DELIM[2] = ";";
#define MAX_DEPTH 6

/**
 * @brief Reads a perft epd file and returns a struct with the parsed data
 * 
 * @param file                  the perft epd file
 * @return struct perft_epd     the parsed contents
 */
struct perft_epd perft_load_file(const char *file) {

    struct perft_epd retval;

    const size_t LINE_SIZE = 300;
    char *line = calloc(LINE_SIZE, sizeof(char));
    FILE *fp;

    fp = fopen(file, "r");

    uint16_t line_cnt = 0;
    while (fgets(line, LINE_SIZE, fp) != NULL) {

        struct epd_row row = parse_row(line);
        retval.rows[line_cnt] = row;
        line_cnt++;

        // init for next time round loop
        memset(line, 0, LINE_SIZE);
    }
    retval.row_count = line_cnt;

    fclose(fp);
    if (line)
        free(line);

    return retval;
}

//////////////////////////////////////////
//
// Private functions
//
//////////////////////////////////////////
struct epd_row parse_row(char *row) {
    struct epd_row retval;

    char *token;

    token = strtok(row, FIELD_DELIM);
    strcpy((char *)(&retval.fen), token);

    uint8_t i = 0;
    while (token != NULL && i < MAX_DEPTH) {
        token = strtok(NULL, FIELD_DELIM);

        // this is of the form "D3 139 "
        // extract the "139"
        uint64_t depth_count = parse_depth_details(token);

        retval.move_cnt[i] = depth_count;

        i++;
    }

    return retval;
}

// extracts the "139" from the following sample string
//     "D3 139 "
uint64_t parse_depth_details(char *str) {
    uint64_t depth;
    char desc[10];

    sscanf(str, "%s %llu", desc, &depth);

    return (uint64_t)depth;
}
