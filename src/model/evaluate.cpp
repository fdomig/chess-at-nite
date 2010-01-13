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

#include "evaluate.h"

int flip[] = {
		112, 113, 114, 115, 116, 117, 118, 119, 0, 0, 0, 0, 0, 0, 0, 0,
		 96,  97,  98,  99, 100, 101, 102, 103, 0, 0, 0, 0, 0, 0, 0, 0,
		 80,  81,  82,  83,  84,  85,  86,  87, 0, 0, 0, 0, 0, 0, 0, 0,
		 64,  65,  66,  67,  68,  69,  70,  71, 0, 0, 0, 0, 0, 0, 0, 0,
		 48,  49,  50,  51,  52,  53,  54,  55, 0, 0, 0, 0, 0, 0, 0, 0,
		 32,  33,  34,  35,  36,  37,  38,  39, 0, 0, 0, 0, 0, 0, 0, 0,
		 16,  17,  18,  19,  20,  21,  22,  23, 0, 0, 0, 0, 0, 0, 0, 0,
		  0,   1,   2,   3,   4,   5,   6,   7, 0, 0, 0, 0, 0, 0, 0, 0,
};

int pawn_table[] = {
		0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
		5, 10, 10,-20,-20, 10, 10,  5, 0, 0, 0, 0, 0, 0, 0, 0,
		5, -5,-10,  0,  0,-10, -5,  5, 0, 0, 0, 0, 0, 0, 0, 0,
		0,  0,  0, 20, 20,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
		5,  5, 10, 25, 25, 10,  5,  5, 0, 0, 0, 0, 0, 0, 0, 0,
		10,10, 20, 30, 30, 20, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0,
		50,50, 50, 50, 50, 50, 50, 50, 0, 0, 0, 0, 0, 0, 0, 0,
		0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int knight_table[] = {
		-50,-40,-30,-30,-30,-30,-40,-50, 0, 0, 0, 0, 0, 0, 0, 0,
		-40,-20,  0,  5,  5,  0,-20,-40, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,  0, 10, 15, 15, 10,  0,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,  5, 15, 20, 20, 15,  5,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,  0, 15, 20, 20, 15,  0,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,  5, 10, 15, 15, 10,  5,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-40,-20,  0,  0,  0,  0,-20,-40, 0, 0, 0, 0, 0, 0, 0, 0,
		-50,-40,-30,-30,-30,-30,-40,-50, 0, 0, 0, 0, 0, 0, 0, 0,
};

int bishop_table[] = {
		-20,-10,-10,-10,-10,-10,-10,-20, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,  5,  0,  0,  0,  0,  5,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-10, 10, 10, 10, 10, 10, 10,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,  0, 10, 10, 10, 10,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,  5,  5, 10, 10,  5,  5,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,  0,  5, 10, 10,  5,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,  0,  0,  0,  0,  0,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-20,-10,-10,-10,-10,-10,-10,-20, 0, 0, 0, 0, 0, 0, 0, 0,
};

int rook_table[] = {
		  0,  0,  0,  5,  5,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
		 -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
		 -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
		 -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
		 -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
		 -5,  0,  0,  0,  0,  0,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
		  5, 10, 10, 10, 10, 10, 10,  5, 0, 0, 0, 0, 0, 0, 0, 0,
		  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int queen_table[] = {
		-20,-10,-10, -5, -5,-10,-10,-20, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,  0,  0,  0,  0,  5,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,  0,  5,  5,  5,  5,  5,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		 -5,  0,  5,  5,  5,  5,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
		  0,  0,  5,  5,  5,  5,  0, -5, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,  0,  5,  5,  5,  5,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,  0,  0,  0,  0,  0,  0,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-20,-10,-10, -5, -5,-10,-10,-20, 0, 0, 0, 0, 0, 0, 0, 0,
};

int king_table_middle[] = {
		 20, 30, 10,  0,  0, 10, 30, 20, 0, 0, 0, 0, 0, 0, 0, 0,
		 20, 20,  0,  0,  0,  0, 20, 20, 0, 0, 0, 0, 0, 0, 0, 0,
		-10,-20,-20,-20,-20,-20,-20,-10, 0, 0, 0, 0, 0, 0, 0, 0,
		-20,-30,-30,-40,-40,-30,-30,-20, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-40,-40,-50,-50,-40,-40,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-40,-40,-50,-50,-40,-40,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-40,-40,-50,-50,-40,-40,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-40,-40,-50,-50,-40,-40,-30, 0, 0, 0, 0, 0, 0, 0, 0,
};

int king_table_end[] = {
		-50,-30,-30,-30,-30,-30,-30,-50, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-30,  0,  0,  0,  0,-30,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-10, 20, 30, 30, 20,-10,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-10, 30, 40, 40, 30,-10,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-10, 30, 40, 40, 30,-10,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-10, 20, 30, 30, 20,-10,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-30,-20,-10,  0,  0,-10,-20,-30, 0, 0, 0, 0, 0, 0, 0, 0,
		-50,-40,-30,-20,-20,-30,-40,-50, 0, 0, 0, 0, 0, 0, 0, 0,
};

#define PENALTY_DOUBLE_PAWN 8
#define BONUS_CASTELING 16
#define END_GAME_LEVEL 1500
#define MATE_SEARCH_LEVEL 600
#define CHECK_BONUS 50

int evaluate(Board* b) {
    // score for both sides
	int score[2] = { 0, 0 };
	int material[2] = { 0, 0 };
	// basic loop to evaluate each piece
	for (int i = 0; i < BOARD_SIZE; i++) {
		if (b->board[i] == EMPTY) {
			continue;
		}
		// white
		if (b->board[i] > 0) {
			switch (b->board[i]) {
				case WHITE_PAWN:
					material[0] += PAWN_VALUE;
					score[0] += evaluate_pawn(b, i);
					break;
				case WHITE_KNIGHT:
					material[0] += KNIGHT_VALUE;
					score[0] += evaluate_knight(b, i);
					break;
				case WHITE_BISHOP:
					material[0] += BISHOP_VALUE;
					score[0] += evaluate_bishop(b, i);
					break;
				case WHITE_ROOK:
					material[0] += ROOK_VALUE;
					score[0] += evaluate_rook(b, i);
					break;
				case WHITE_QUEEN:
					material[0] += QUEEN_VALUE;
					score[0] += evaluate_queen(b, i);
					break;
				case WHITE_KING:
					break;
			}
		} else {
			switch (b->board[i]) {
				case BLACK_PAWN:
					material[1] += PAWN_VALUE;
					score[1] += evaluate_pawn(b, i);
					break;
				case BLACK_KNIGHT:
					material[1] += KNIGHT_VALUE;
					score[1] += evaluate_knight(b, i);
					break;
				case BLACK_BISHOP:
					material[1] += BISHOP_VALUE;
					score[1] += evaluate_bishop(b, i);
					break;
				case BLACK_ROOK:
					material[1] += ROOK_VALUE;
					score[1] += evaluate_rook(b, i);
					break;
				case BLACK_QUEEN:
					material[1] += QUEEN_VALUE;
					score[1] += evaluate_queen(b, i);
					break;
				case BLACK_KING:
					break;
			}
		}
	}

	// mate level?
	if (material[0] <= MATE_SEARCH_LEVEL || material[1] <= MATE_SEARCH_LEVEL) {
		MoveGenerator g = MoveGenerator(b);
		if (g.check_for_check_simple(WHITE)) {
			score[1] += CHECK_BONUS;
		} else if (g.check_for_check_simple(BLACK)) {
			score[0] += CHECK_BONUS;
		}
	}

	// evaluate kings
	score[0] += evaluate_king(b, b->white_king, material[0], material[1]);
	score[1] += evaluate_king(b, b->black_king, material[0], material[1]);


	// special moves
	// TODO: not all implemented

	// Castling bonus
	if (b->to_move == BLACK
			&& (b->history.back().m.special == MOVE_CASTLE_LONG
				|| b->history.back().m.special == MOVE_CASTLE_SHORT)) {
		score[0] += b->history.back().m.special * BONUS_CASTELING;
	}
	if (b->to_move == WHITE
			&& (b->history.back().m.special == MOVE_CASTLE_LONG
				|| b->history.back().m.special == MOVE_CASTLE_SHORT)) {
		score[1] += b->history.back().m.special * BONUS_CASTELING;
	}

	score[0] += material[0];
	score[1] += material[1];

	// final score is relative to the side to move
	return b->to_move * (score[0] - score[1]);
}

int evaluate_pawn(Board* b, int sq) {
	if (b->board[sq] > 0) {
		// add a penalty if there is another friendly pawn behind this
		int score = pawn_table[sq];
		if (b->board[sq - NEXT_RANK] == WHITE_PAWN) {
			score += PENALTY_DOUBLE_PAWN;
		}
		return score;
	}
	int score = pawn_table[flip[sq]];
	if (b->board[sq + NEXT_RANK] == BLACK_PAWN) {
		score += PENALTY_DOUBLE_PAWN;
	}
	return score;
}

int evaluate_knight(Board* b, int sq) {
	if (b->board[sq] > 0) {
		return knight_table[sq];
	}
	return knight_table[flip[sq]];
}

int evaluate_bishop(Board* b, int sq) {
	if (b->board[sq] > 0) {
		return bishop_table[sq];
	}
	return bishop_table[flip[sq]];
}

int evaluate_rook(Board* b, int sq) {
	if (b->board[sq] > 0) {
		return rook_table[sq];
	}
	return rook_table[flip[sq]];
}

int evaluate_queen(Board* b, int sq) {
	if (b->board[sq] > 0) {
		return queen_table[sq];
	}
	return queen_table[flip[sq]];
}

int evaluate_king(Board* b, int sq, int material_white, int material_black) {
	int score = 0;

	// WHITE
	if (b->board[sq] > 0) {
		if (material_white > END_GAME_LEVEL) {
			score += king_table_middle[sq];
		} else {
			score +=  king_table_end[sq];
		}

		// calculate king safety
		score *= material_black;
		score /= 3100;

	} else {
		// BLACK
		if (material_black > END_GAME_LEVEL) {
			score += king_table_middle[flip[sq]];
		} else {
			score += king_table_end[flip[sq]];
		}
	}

	// calculate king safety
	score *= material_black;
	score /= 3100;

	return score;
}

int repetitions(Board* b) {
	int r = 0;
	int first = b->history.size() - b->fifty_moves;
	if (first >=0) {
        for (int i = first; i < ((int)b->history.size() - 1); i++) {
            if (b->history[i].hash == b->get_hash()) {
                r++;
            }
        }
	}
	return r;
}
