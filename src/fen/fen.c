#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "piece.h"
#include "square.h"
#include "position.h"
#include "fen.h"

#define MAX_MOVE_DIGITS		4
#define MAX_FEN 			255
static const char space_delim[] 	= " ";
static const char renk_delim[] 		= "/";

static void init_parsed_fen(struct parsed_fen *pf);
static void add_piece(struct parsed_fen *pf, const enum square sq, const enum piece pce);
static void set_side_to_move(struct parsed_fen *pf, const enum colour side);
static void set_castle_permission(struct parsed_fen *pf, const enum castle_perm cp);
static void setup_piece_positions(struct parsed_fen *pf, const char *pieces);
static void setup_side_to_move(struct parsed_fen *pf, const char *side);
static void setup_castle_permissions(struct parsed_fen *pf, const char *perms);
static void setup_en_passant_sq(struct parsed_fen *pf, const char *en_pass);
static void setup_half_move_count(struct parsed_fen *pf, const char * half_move_cnt);
static void setup_half_move_count(struct parsed_fen *pf, const char * full_move_cnt);
static uint16_t convert_move_count(char * str);




struct piece_location {
	enum piece piece;
	bool is_occupied;
};


struct parsed_fen {
	struct piece_location pieces[NUM_SQUARES];

	enum colour side_to_move;

	bool is_castle_perm_set;
	cast_perm_t castle_perm;

	bool is_en_pass_set;
	enum square en_pass_sq;

	uint16_t half_move_cnt;
	uint16_t full_move_cnt;
};

// there's only 1
static struct parsed_fen decomposed_fen;



// rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1

struct parsed_fen* parse_fen(const char* fen_string)
{
	const char space_delim[] = " ";

	// copy for tokenising
	char fen[MAX_FEN];
	memcpy(fen, fen_string, strlen(fen));

	// set up return value
	struct parsed_fen* retval = &decomposed_fen;
	init_parsed_fen(retval);


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

	return retval;
}

bool try_get_piece_on_sq(const struct parsed_fen* pf, const enum square sq, enum piece *pce)
{
	if (pf->pieces[sq].is_occupied == true) {
		*pce = pf->pieces[sq].piece;
		return true;
	}
	return false;
}

bool try_get_castle_permissions(const struct parsed_fen* pf, cast_perm_t *cp)
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
		*sq = pf->en_pass_sq;
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
	memset(pf, 0, sizeof(struct parsed_fen));

	for (int i = 0; i < NUM_SQUARES; i++) {
		pf->pieces[NUM_SQUARES].is_occupied = false;
	}

	pf->side_to_move = WHITE;
	pf->is_castle_perm_set = false;
	pf->castle_perm = CAST_PERM_NONE;
	pf->is_en_pass_set = false;
}



static void setup_piece_positions(struct parsed_fen *pf, const char *pieces)
{
	uint8_t rank = RANK_8;
	uint8_t file = FILE_A;
	uint8_t count = 0;

	while ((rank >= RANK_1) && *pieces) {
		enum piece piece_to_add;
		bool piece_found = true;
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
			count = (*pieces) - '0';
			piece_found = false;
			break;

		case '/':
			rank--;
			file = FILE_A;
			pieces++;
			piece_found = false;
			continue;

		default:
			printf("FEN error \n");
			return;
		}

		if (piece_found == true) {
			enum square sq = get_square((enum rank)rank, (enum file)file);
			pf->pieces[sq].is_occupied = true;
			pf->pieces[sq].piece = piece_to_add;
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
		pf->castle_perm = CAST_PERM_NONE;
	} else {
		for (int i = 0; i < strlen(perms); i++) {
			switch (*perms) {
			case 'K':
				add_
				pf->castle_perm |= CAST_PERM_WK;
				break;
			case 'Q':
				pf->castle_perm |= CAST_PERM_WK;
				break;
			case 'k':
				pf->castle_perm |= CAST_PERM_BK;
				break;
			case 'q':
				pf->castle_perm |= CAST_PERM_BQ;
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


static void setup_half_move_count(struct parsed_fen *pf, const char *half_move_cnt)
{
	uint16_t half_move = convert_move_count(half_move_cnt);
	pf->half_move_cnt = half_move_cnt;
}


static void setup_half_move_count(struct parsed_fen *pf, const char *full_move_cnt)
{
	uint16_t full_move = convert_move_count(full_move_cnt);
	pf->half_move_cnt = full_move;
}




static uint16_t convert_move_count(char * str)
{
	char *eptr;
	long result;

	result = strtol(str, &eptr, MAX_MOVE_DIGITS);

	/* If the result is 0, test for an error */
	if (result == 0) {
		if (errno == EINVAL) {
			printf("Conversion error occurred: %d\n", errno);
			exit(0);
		}

		if (errno == ERANGE)
			printf("The value provided was out of range\n");
	}
	return (uint16_t)result;
}


