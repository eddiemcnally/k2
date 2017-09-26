#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "piece.h"
#include "square.h"
#include "position.h"
#include "fen.h"


static const uint8_t MAX_FEN 		= 255;
static const char space_delim[] = " ";
static const char renk_delim[] = "/";

static void init_parsed_fen(struct parsed_fen *pf)
static void add_piece(struct parsed_fen *pf, const enum square sq, const enum piece pce)
static void set_side_to_move(struct parsed_fen *pf, const enum colour side)
static void set_castle_permission(struct parsed_fen *pf, const enum castle_perms cp)
static void setup_piece_positions(struct parsed_fen *pf, const char *pieces)
static void setup_side_to_move(struct parsed_fen *pf, const char *side)
static void setup_castle_permissions(struct parsed_fen *pf, const char *perms)
static void setup_en_passant_sq(struct parsed_fen *pf, const char *en_pass)
static void setup_half_move_count(retval, half_move_cnt)
static void setup_half_move_count(retval, full_move_cnt)




static struct piece_location {
	enum piece piece;
	bool is_occupied;
}


static struct parsed_fen {
	struct piece_location pieces[NUM_SQUARES];

	enum colour side_to_move;

	bool is_castle_perm_set;
	cast_perm_t castle_perm;

	bool is_en_pass_set;
	enum square en_pass_sq;

	uint16_t half_move_cnt;
	uint16_t full_move_cnt;

}

// there's only 1
static struct parsed_fen decomposed_fen;



// rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1

struct parsed_fen* parse_fen(const char* fen_string)
{
	const char space_delim[] = " ";

	// copy for tokenising
	char fen[MAX_FEN];
	memcpy(temp_fen, fen_string, strlen(fen));

	init_parsed_fen(retval);

	// set up return value
	struct parsed_fen* retval = &decomposed_fen;

	// split into main sections
	char* pieces = strtok (fen, space_delim);
	char* side = strtok (NULL, space_delim);
	char* cast_perms = strtok (NULL, space_delim);
	char* en_pass = strtok (NULL, space_delim);
	char* half_move_cnt = strtok (NULL, space_delim);
	char* full_move_cnt = strtok (NULL, space_delim);


	setup_piece_positions(retval, pieces);
	setup_side_to_move(retval, side);
	setup_castle_permissions(retval, cast_perms);
	setup_en_passant_sq(retval, en_pass);
	setup_half_move_count(retval, half_move_cnt);
	setup_half_move_count(retval, full_move_cnt);
}

bool try_get_piece_on_sq(const struct parsed_fen* pf, const enum square sq, enum piece *pce)
{
	if (pf->pieces[sq].is_occupied == true) {
		*pce = pf->pieces[sq].piece;
		return true;
	}
	return false;
}

bool try_get_castle_permissions(const struct parsed_fen* pf, enum castle_perms *cp)
{
	if (pf->is_castle_perm_set == true) {
		*cp = pf->castle_perm;
		return true;
	}
	return false;
}

bool try_get_en_pass_sq(const struct parsed_fen* pf, enum square *sq)
{
	if (pf->is_en_pass_set == true) {
		*cp = pf->en_pass_sq;
		return true;
	}
	return false;
}


enum colour get_side_to_move(const struct parsed_fen* pf)
{
	return pf->side_to_move;
}

uint16_t get_half_move_cnt(const struct parsed_fen* pf)
{
	return pf->half_move_cnt;
}
uint16_t get_fill_move_cnt(const struct parsed_fen* pf)
{
	return pf->full_move_cnt;
}


//---------------------------------------------------------
// static functions below
//---------------------------------------------------------

static void init_parsed_fen(struct parsed_fen *pf)
{
	memset(pf, 0, size_of(struct parsed_fen));

	for (int i = 0; i < NUM_SQUARES; i++) {
		pf->pieces[NUM_SQUARES].is_occupied = false;
	}

	pf->side_to_move = WHITE;
	pf->is_castle_perm_set = false;
	pf->castle_perm = CAST_PERM_NONE;
	pf->is_en_pass_set = false;
}


static void add_piece(struct parsed_fen *pf, const enum square sq, const enum piece pce)
{
	pf->pieces[sq] = pce;
}

static void set_side_to_move(struct parsed_fen *pf, const enum colour side)
{
	pf->side_to_move = side;
}

static void set_castle_permission(struct parsed_fen *pf, const enum castle_perms cp)
{
	pf->castle_perm = cp;
}


static void setup_piece_positions(struct parsed_fen *pf, const char *pieces)
{
	uint8_t rank = RANK_8;
	uint8_t file = FILE_A;
	uint8_t count = 0;

	while ((rank >= RANK_1) && *pieces) {
		enum piece piece_to_add = NOT_POPULATED;
		count = 1;

		switch (*pieces) {
		case 'p':
			piece_to_add = BPAWN;
			break;
		case 'r':
			piece_to_add = BROOK;
			break;
		case 'n':
			piece_to_add = BKNIGHT;
			break;
		case 'b':
			piece_to_add = BBISHOP;
			break;
		case 'q':
			piece_to_add = BQUEEN;
			break;
		case 'k':
			piece_to_add = BKING;
			break;
		case 'P':
			piece_to_add = WPAWN;
			break;
		case 'R':
			piece_to_add = WROOK;
			break;
		case 'N':
			piece_to_add = WKNIGHT;
			break;
		case 'B':
			piece_to_add = WBISHOP;
			break;
		case 'Q':
			piece_to_add = WQUEEN;
			break;
		case 'K':
			piece_to_add = WKING;
			break;

		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			count = (*fen) - '0';
			break;

		case '/':
			rank--;
			file = FILE_A;
			pieces++;
			continue;

		default:
			printf("FEN error \n");
			return;
		}

		for (int i = 0; i < count; i++) {
			if (piece_to_add != NOT_POPULATED) {
				enum square sq = get_square((enum rank)rank, (enum file)file);
				add_piece(retval, sq, piece_to_add);
			}
			file++;
		}
		pieces++;
	}
}


static void setup_side_to_move(struct parsed_fen *pf, const char *side)
{
	if (*side == 'w') {
		set_side_to_move(retval, WHITE);
	} else {
		set_side_to_move(retval, BLACK);
	}
}


static void setup_castle_permissions(struct parsed_fen *pf, const char *perms)
{
	if (*perms == '-') {
		pf->castle_perm = CAST_PERM_NONE;
	} else {
		for (int i = 0; i < strlen(perms); i++) {
			switch (*perms) {
			case 'K':
				set_castle_permission(retval, CAST_PERM_WK);
				break;
			case 'Q':
				set_castle_permission(retval, CAST_PERM_WK);
				break;
			case 'k':
				set_castle_permission(retval, CAST_PERM_BK);
				break;
			case 'q':
				set_castle_permission(retval, CAST_PERM_BQ);
				break;
			default:
				break;
			}
			perms++;
		}
	}
}


static void setup_en_passant_sq(struct parsed_fen *pf, const char *en_pass)
{
	if (*en_pass != '-') {
		// en passant square present
		file = en_pass[0] - 'a';
		rank = en_pass[1] - '1';

		pf->is_en_pass_set = true;
		pf->en_pass_sq = get_square((enum rank)rank, (enum file)file));
	} else {
		pf->is_en_pass_set = false;
	}
}


static void setup_half_move_count(retval, half_move_cnt)
{

}


static void setup_half_move_count(retval, full_move_cnt)
{

}



