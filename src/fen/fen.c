#include "fen.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "position.h"
#include "square.h"

#define MAX_FEN 255

static void init_parsed_fen(struct parsed_fen *pf);
static void setup_piece_positions(struct parsed_fen *pf, const char *pieces);
static void setup_side_to_move(struct parsed_fen *pf, const char *side);
static void setup_castle_permissions(struct parsed_fen *pf, const char *perms);
static void setup_en_passant_sq(struct parsed_fen *pf, const char *en_pass);
static void setup_half_move_count(struct parsed_fen *pf, const char *half_move_cnt);
static void setup_full_move_count(struct parsed_fen *pf, const char *full_move_cnt);
static uint16_t convert_move_count(const char *str);
static void handle_rank(struct parsed_fen *pf, const enum rank rank, const char *pieces);

struct piece_location {
	enum piece piece;
	bool is_occupied;
};

struct parsed_fen {
	struct piece_location pieces[NUM_SQUARES];

	enum colour side_to_move;

	uint8_t castle_perm;

	bool is_en_pass_set;
	enum square en_pass_sq;

	uint16_t half_move_cnt;
	uint16_t full_move_cnt;
};

// there's only 1
static struct parsed_fen decomposed_fen;

/**
 * @brief Parses the given FEN
 * @details takes a FEN string, parses it and returns a ptr to a struct with the parses data
 *
 * @param fen_string FEN-formatted string
 * @return Pointer to struct parsed_fen
 */
struct parsed_fen *parse_fen(const char *fen_string)
{
	char space_delim[] = " ";

	// copy for tokenising
	char fen[MAX_FEN] = {0};

	memcpy(fen, fen_string, strlen(fen_string));

	// set up return value
	struct parsed_fen *retval = &decomposed_fen;
	init_parsed_fen(retval);

	// split into main sections
	char *pieces = strtok(fen, space_delim);
	char *side = strtok(NULL, space_delim);
	char *cast_perms = strtok(NULL, space_delim);
	char *en_pass = strtok(NULL, space_delim);
	char *half_move_cnt = strtok(NULL, space_delim);
	char *full_move_cnt = strtok(NULL, space_delim);

	setup_piece_positions(retval, pieces);
	setup_side_to_move(retval, side);
	setup_castle_permissions(retval, cast_perms);
	setup_en_passant_sq(retval, en_pass);
	setup_half_move_count(retval, half_move_cnt);
	setup_full_move_count(retval, full_move_cnt);

	return retval;
}

/**
 * @brief Returns the piece on the given square
 * @details Tries to extract the piece on the given square using the parsed_fen struct
 *
 * @param parsed_fen The struct containing the parsed FEN
 * @param square The square
 * @param piece Pointer where returned piece will be saved
 * @return true if piece found, false otherwise
 */
bool try_get_piece_on_sq(const struct parsed_fen *pf, const enum square sq,
                         enum piece *pce)
{
	if (pf->pieces[sq].is_occupied == true) {
		*pce = pf->pieces[sq].piece;
		//printf("returning piece %c\n", get_label(*pce));
		return true;
	}
	return false;
}

/**
 * @brief Gets the castle permissions
 * @details Takes the passed in *struct and returns the castle permissions
 *
 * @param parsed_fen The ptr to a parsed_fen struct
 *
 * @return the castle permissions
 */
uint8_t get_castle_permissions(const struct parsed_fen *pf)
{
	return pf->castle_perm;
}

bool try_get_en_pass_sq(const struct parsed_fen *pf, enum square *sq)
{
	if (pf->is_en_pass_set == true) {
		*sq = pf->en_pass_sq;
		return true;
	}
	return false;
}

enum colour get_side_to_move(const struct parsed_fen *pf)
{
	return pf->side_to_move;
}

uint16_t get_half_move_cnt(const struct parsed_fen *pf)
{
	return pf->half_move_cnt;
}
uint16_t get_fill_move_cnt(const struct parsed_fen *pf)
{
	return pf->full_move_cnt;
}

//---------------------------------------------------------
// static functions below
//---------------------------------------------------------

static void init_parsed_fen(struct parsed_fen *pf)
{
	memset(pf, 0, sizeof(struct parsed_fen));

	struct piece_location *plocs = pf->pieces;
	for (int i = 0; i < NUM_SQUARES; i++) {
		plocs->is_occupied = false;
		plocs++;
	}

	pf->side_to_move = WHITE;
	pf->castle_perm = CAST_PERM_NONE;
	pf->is_en_pass_set = false;
}

static void setup_piece_positions(struct parsed_fen *pf, const char *pieces)
{
	char rank_delim[] = "/";

	// copy for tokenising
	char tmp[MAX_FEN] = {0};

	memcpy(tmp, pieces, strlen(pieces));

	// split into ranks sections
	char *rank8 = strtok(tmp, rank_delim);
	handle_rank(pf, RANK_8, rank8);

	for (int r = RANK_7; r >= RANK_1; r--) {
		char *rank_str = strtok(NULL, rank_delim);
		handle_rank(pf, (enum rank)r, rank_str);
	}
}

static void handle_rank(struct parsed_fen *pf, const enum rank rank,
                        const char *pieces)
{
	enum file file = FILE_A;

	while (*pieces) {
		enum piece piece_to_add;
		bool piece_found = true;

		char c = *pieces;

		if (isdigit(c)) {
			//printf("found number %c\n", c);
			file += (uint8_t)((c) - '0');
			piece_found = false;
		} else {
			piece_to_add = get_piece_from_label(c);
		}

		if (piece_found == true) {
			//printf("found piece %c\n", c);
			enum square sq = get_square((enum rank)rank, (enum file)file);

			//printf("adding piece %c to square %s\n", c, print_square(sq));

			pf->pieces[sq].is_occupied = true;
			pf->pieces[sq].piece = piece_to_add;
			file++;
		}

		pieces++;
	}
}

static void setup_side_to_move(struct parsed_fen *pf, const char *side)
{
	if (*side == 'w') {
		pf->side_to_move = WHITE;
	} else {
		pf->side_to_move = BLACK;
	}
}

static void setup_castle_permissions(struct parsed_fen *pf, const char *perms)
{
	if (*perms == '-') {
		add_cast_perm(&pf->castle_perm, CAST_PERM_NONE);
		return;
	}

	uint8_t len = (uint8_t)strlen(perms);

	for (int i = 0; i < len; i++) {
		uint8_t cp = CAST_PERM_NONE;

		switch (*perms) {
		case 'K':
			cp = CAST_PERM_WK;
			break;
		case 'Q':
			cp = CAST_PERM_WQ;
			break;
		case 'k':
			cp = CAST_PERM_BK;
			break;
		case 'q':
			cp = CAST_PERM_BQ;
			break;
		default:
			assert(true);
			printf("FEN.C : invalid Castle Permission character %c\n", *perms);
			break;
		}

		add_cast_perm(&pf->castle_perm, cp);
		perms++;
	}
}

static void setup_en_passant_sq(struct parsed_fen * pf, const char *en_pass)
{
	if (*en_pass != '-') {
		// en passant square present
		int file = en_pass[0] - 'a';
		int rank = en_pass[1] - '1';

		pf->en_pass_sq = get_square((enum rank)rank, (enum file)file);
	} else {
		pf->is_en_pass_set = false;
	}
}

static void setup_half_move_count(struct parsed_fen * pf,
                                  const char *half_move_cnt)
{
	uint16_t half_move = convert_move_count(half_move_cnt);
	pf->half_move_cnt = half_move;
}

static void setup_full_move_count(struct parsed_fen * pf,
                                  const char *full_move_cnt)
{
	uint16_t full_move = convert_move_count(full_move_cnt);
	pf->half_move_cnt = full_move;
}

static uint16_t convert_move_count(const char *str)
{
	int result = atoi(str);
	return (uint16_t)result;
}
