/*
 * This file is part of the chess-at-nite project.
 *
 * Copyright (c) 2009 by
 *   Franziskus Domig
 *   Panayiotis Lipiridis
 *   Radoslav Petrik
 *   Thai Gia Tuong
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef DEFINE_H_
#define DEFINE_H_

#define VERSION "0.6.1"

//defines for different compiles

#define TOURNAMENT_
#define SHOW_THINKING
#define DEBUG_
#define SHOW_BEST_SCORE
#define USE_HASH_TABLE_

#ifndef WIN32
#define UNICODE_
#endif

#define COMMAND_LINE

#define OPENING_BOOK "book"
#define WAC_FILE "wac.ci"

#define BOARD_SIZE 128
#define SIZE 8

#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define NO_SQUARE -1

#define EVALUATION_START 50000

// castling options bitwise
#define CASTLE_NONE   0
#define CASTLE_SHORT  1
#define CASTLE_LONG   2

#define WHITE_KING_INIT_SQUARE  E1
#define BLACK_KING_INIT_SQUARE  E8

#define WHITE_ROOK_LONG_SQUARE  A1
#define WHITE_ROOK_SHORT_SQUARE H1
#define BLACK_ROOK_LONG_SQUARE  A8
#define BLACK_ROOK_SHORT_SQUARE H8

// constants for type of move
#define MOVE_ORDINARY           0
#define MOVE_CASTLE_SHORT       CASTLE_SHORT
#define MOVE_CASTLE_LONG        CASTLE_LONG
#define MOVE_EN_PASSANT         3
#define MOVE_PROMOTION          4

//when asking from user....
#define MOVE_ERROR             5
#define MOVE_UNDO              6
#define MOVE_RESIGN            7
#define MOVE_DRAW              8
#define MOVE_SHOW_MOVES        9
#define MOVE_SHOW_HISOTRY     10
#define MOVE_SHOW_BOARD       11
#define MOVE_SHOW_HELP        12

//for the status of the game!
#define STATUS_NORMAL          0
#define STATUS_CHECK           1
#define STATUS_CHECKMATE       2
#define STATUS_STALEMATE       3
#define STATUS_DRAW            4

// side
#define WHITE  1
#define BLACK -1
#define OPPONENT(color) (-1 * (color))

// evaluation values
#define QUEEN_VALUE     900
#define ROOK_VALUE      500
#define BISHOP_VALUE    300
#define KNIGHT_VALUE    300
#define PAWN_VALUE      100

#define MATE          24000
#define DRAW              0

// pieces
//!! don't change !!!
#define EMPTY	0
#define PAWN    1
#define KNIGHT  2
#define BISHOP  3
#define ROOK    4
#define QUEEN   5
#define KING    6

#define WHITE_PAWN    (WHITE * PAWN)
#define WHITE_KNIGHT  (WHITE * KNIGHT)
#define WHITE_BISHOP  (WHITE * BISHOP)
#define WHITE_ROOK    (WHITE * ROOK)
#define WHITE_QUEEN   (WHITE * QUEEN)
#define WHITE_KING    (WHITE * KING)

#define BLACK_PAWN    (BLACK * PAWN)
#define BLACK_KNIGHT  (BLACK * KNIGHT)
#define BLACK_BISHOP  (BLACK * BISHOP)
#define BLACK_ROOK    (BLACK * ROOK)
#define BLACK_QUEEN   (BLACK * QUEEN)
#define BLACK_KING    (BLACK * KING)

// row/column distance in the board
#define NEXT_RANK 16
#define NEXT_FILE  1

//castling distances
#define CASTLING_SHORT_DIST_ROOK 3
#define CASTLING_LONG_DIST_ROOK 4

//directions
#define MV_U   NEXT_RANK //move up
#define MV_D  -NEXT_RANK //move down
#define MV_L  -NEXT_FILE //move left
#define MV_R   NEXT_FILE //move right
#define MV_UL (MV_U + MV_L) //move up left
#define MV_UR (MV_U + MV_R) //move up right
#define MV_DL (MV_D + MV_L) //move down left
#define MV_DR (MV_D + MV_R) //move down right

#define MAX_THINKING_TIME 15000 //ms
#define MAX_SEARCH_DEPTH 20

// do not change!
#define MAX_PLY 64

// typedef for a byte
typedef signed char byte;

// squares for the 0x88 board
typedef enum _SQUARE {
	A1 = 0,
	A2 = 16,
	A3 = 32,
	A4 = 48,
	A5 = 64,
	A6 = 80,
	A7 = 96,
	A8 = 112,
	B1 = 1,
	B2 = 17,
	B3 = 33,
	B4 = 49,
	B5 = 65,
	B6 = 81,
	B7 = 97,
	B8 = 113,
	C1 = 2,
	C2 = 18,
	C3 = 34,
	C4 = 50,
	C5 = 66,
	C6 = 82,
	C7 = 98,
	C8 = 114,
	D1 = 3,
	D2 = 19,
	D3 = 35,
	D4 = 51,
	D5 = 67,
	D6 = 83,
	D7 = 99,
	D8 = 115,
	E1 = 4,
	E2 = 20,
	E3 = 36,
	E4 = 52,
	E5 = 68,
	E6 = 84,
	E7 = 100,
	E8 = 116,
	F1 = 5,
	F2 = 21,
	F3 = 37,
	F4 = 53,
	F5 = 69,
	F6 = 85,
	F7 = 101,
	F8 = 117,
	G1 = 6,
	G2 = 22,
	G3 = 38,
	G4 = 54,
	G5 = 70,
	G6 = 86,
	G7 = 102,
	G8 = 118,
	H1 = 7,
	H2 = 23,
	H3 = 39,
	H4 = 55,
	H5 = 71,
	H6 = 87,
	H7 = 103,
	H8 = 119
} square;

/* 32-bit number to hold the whole information of a move
 * allows the move to be passed to the stack without pointer logic
 *
 * During the en passant move.. the pos_new has the position of the captured piece
 * and the board knows where to put the actual piece that was moved!
 */
typedef union _MOVE {
		unsigned int move;
		struct {
				unsigned char pos_old :8;
				unsigned char pos_new :8;
				unsigned char special :4; // all the special moves defined above
				signed char moved_piece :4;
				signed char content :4;
				signed char promoted :4;
		};
} move;

typedef struct _HISTORY_ITEM {
		move m;
		int white_castle;
		int black_castle;
		int en_passant;
		int fifty_moves;
		int hash;
} history_item;

#ifdef USE_HASH_TABLE
// hash table
#define HT_SIZE (1<<24)

enum htype {
	NO, EXACT, LOWER, UPPER
};

typedef struct {
		int score;
		int depth;
		int wtm;
		int key;
		move best;
		htype type;

} htentry;
#endif

// rank and file value of a square
#define RANK(s) (s >> 4)
#define FILE(s) (s & 7)

//opposite direction
#define OPPOSITE(a) (~a+1)

// unicode border symbols for a better user experience
#define B_RD  "\u2554"
#define B_LD  "\u2557"
#define B_LU  "\u255D"
#define B_RU  "\u255A"

#define B_LR  "\u2550"
#define B_UD  "\u2551"

#define B_UDR "\u2560"
#define B_UDL "\u2563"
#define B_LRD "\u2566"
#define B_LRU "\u2569"

#define B_LRUD "\u256C"

// include utils
#include "utils.h"
#include <stdlib.h>

#endif /* DEFINE_H_ */
