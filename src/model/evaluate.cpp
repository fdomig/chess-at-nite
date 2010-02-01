/*
 * This file is part of the chess-at-nite project [chess-at-nite.googlecode.com]
 *
 * Copyright (c) 2009-2010 by
 *   Franziskus Domig
 *   Panayiotis Lipiridis
 *   Radoslav Petrik
 *   Thai Gia Tuong
 *
 * For the full copyright and license information, please visit:
 *   http://chess-at-nite.googlecode.com/svn/trunk/doc/LICENSE
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
      0,   0,  0,    0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
      5,  10,  10, -20, -20,  10,  10,   5, 0, 0, 0, 0, 0, 0, 0, 0,
      5,  -5, -10,   0,   0, -10,  -5,   5, 0, 0, 0, 0, 0, 0, 0, 0,
      0,   0,   0,  20,  20,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
      5,   5,  10,  25,  25,  10,   5,   5, 0, 0, 0, 0, 0, 0, 0, 0,
     10,  10,  20,  30,  30,  20,  10,  10, 0, 0, 0, 0, 0, 0, 0, 0,
     50,  50,  50,  50,  50,  50,  50,  50, 0, 0, 0, 0, 0, 0, 0, 0,
      0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int knight_table[] = {
    -50, -40, -30, -30, -30, -30, -40, -50, 0, 0, 0, 0, 0, 0, 0, 0,
    -40, -20,   0,   5,   5,   0, -20, -40, 0, 0, 0, 0, 0, 0, 0, 0,
    -30,   0,  10,  15,  15,  10,   0, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30,   5,  15,  20,  20,  15,   5, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30,   0,  15,  20,  20,  15,   0, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30,   5,  10,  15,  15,  10,   5, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -40, -20,   0,   0,   0,   0, -20, -40, 0, 0, 0, 0, 0, 0, 0, 0,
    -50, -40, -30, -30, -30, -30, -40, -50, 0, 0, 0, 0, 0, 0, 0, 0,
};

int bishop_table[] = {
    -20, -10, -10, -10, -10, -10, -10, -20, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,   5,   0,   0,   0,   0,   5, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,  10,  10,  10,  10,  10,  10, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,   0,  10,  10,  10,  10,   0, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,   5,   5,  10,  10,   5,   5, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,   0,   5,  10,  10,   5,   0, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,   0,   0,   0,   0,   0,   0, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -20, -10, -10, -10, -10, -10, -10, -20, 0, 0, 0, 0, 0, 0, 0, 0,
};

int rook_table[] = {
      0,   0,   0,   5,   5,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
     -5,   0,   0,   0,   0,   0,   0,  -5, 0, 0, 0, 0, 0, 0, 0, 0,
     -5,   0,   0,   0,   0,   0,   0,  -5, 0, 0, 0, 0, 0, 0, 0, 0,
     -5,   0,   0,   0,   0,   0,   0,  -5, 0, 0, 0, 0, 0, 0, 0, 0,
     -5,   0,   0,   0,   0,   0,   0,  -5, 0, 0, 0, 0, 0, 0, 0, 0,
     -5,   0,   0,   0,   0,   0,   0,  -5, 0, 0, 0, 0, 0, 0, 0, 0,
      5,  10,  10,  10,  10,  10,  10,   5, 0, 0, 0, 0, 0, 0, 0, 0,
      0,   0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
};

int queen_table[] = {
    -20, -10, -10,  -5,  -5, -10, -10, -20, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,   0,   0,   0,   0,   5,   0, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,   0,   5,   5,   5,   5,   5, -10, 0, 0, 0, 0, 0, 0, 0, 0,
     -5,   0,   5,   5,   5,   5,   0,  -5, 0, 0, 0, 0, 0, 0, 0, 0,
      0,   0,   5,   5,   5,   5,   0,  -5, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,   0,   5,   5,   5,   5,   0, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -10,   0,   0,   0,   0,   0,   0, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -20, -10, -10,  -5,  -5, -10, -10, -20, 0, 0, 0, 0, 0, 0, 0, 0,
};

int king_table_middle[] = {
     20,  30,  10,   0,   0,  10,  30,  20, 0, 0, 0, 0, 0, 0, 0, 0,
     20,  20,   0,   0,   0,   0,  20,  20, 0, 0, 0, 0, 0, 0, 0, 0,
    -10, -20, -20, -20, -20, -20, -20, -10, 0, 0, 0, 0, 0, 0, 0, 0,
    -20, -30, -30, -40, -40, -30, -30, -20, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -40, -40, -50, -50, -40, -40, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -40, -40, -50, -50, -40, -40, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -40, -40, -50, -50, -40, -40, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -40, -40, -50, -50, -40, -40, -30, 0, 0, 0, 0, 0, 0, 0, 0,
};

int king_table_end[] = {
    -50, -30, -30, -30, -30, -30, -30, -50, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -30,   0,   0,   0,   0, -30, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -10,  20,  30,  30,  20, -10, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -10,  30,  40,  40,  30, -10, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -10,  30,  40,  40,  30, -10, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -10,  20,  30,  30,  20, -10, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -30, -20, -10,   0,   0, -10, -20, -30, 0, 0, 0, 0, 0, 0, 0, 0,
    -50, -40, -30, -20, -20, -30, -40, -50, 0, 0, 0, 0, 0, 0, 0, 0,
};

#define PENALTY_DOUBLE_PAWN      -8
#define BONUS_CASTLING           16
#define BONUS_CHECK              50
#define BONUS_ENDGAME_PAWN_MOVE  50
#define END_GAME_LEVEL         1500
#define MATE_SEARCH_LEVEL       600

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
            score += king_table_end[sq];
        }

        // calculate king safety
        score *= material_black;
        //TODO: define the 3100.. what is that number???!!
        score /= 3100;
    } else {
        // BLACK
        if (material_black > END_GAME_LEVEL) {
            score += king_table_middle[flip[sq]];
        } else {
            score += king_table_end[flip[sq]];
        }
        // calculate king safety
        score *= material_white;
        //TODO: define the 3100.. what is that number???!!
        score /= 3100;
    }
    return score;
}

int repetitions(Board* b) {
    int r = 0;
    int first = b->history.size() - b->fifty_moves;
    if (first >= 0) {
        for (int i = first; i < ((int) b->history.size() - 1); i++) {
            if (b->history[i].hash == b->get_hash()) {
                r++;
            }
        }
    }
    return r;
}

int evaluate(Board* b) {
    // score for both sides
    int score_white = 0;
    int score_black = 0;
    // material values for both sides
    int material_white = 0;
    int material_black = 0;
    // basic loop to evaluate each piece
    for (int square = 0; square < BOARD_SIZE; ++square) {
        //skip rightmost place of the board7 (always empty)
        if ((square & 15) == 8) // &15 = mod 16
            square += 8;
        if (b->board[square] == EMPTY) {
            continue;
        }

        switch (b->board[square]) {
            case WHITE_PAWN:
                material_white += PAWN_VALUE;
                score_white += evaluate_pawn(b, square);
                break;
            case WHITE_KNIGHT:
                material_white += KNIGHT_VALUE;
                score_white += evaluate_knight(b, square);
                break;
            case WHITE_BISHOP:
                material_white += BISHOP_VALUE;
                score_white += evaluate_bishop(b, square);
                break;
            case WHITE_ROOK:
                material_white += ROOK_VALUE;
                score_white += evaluate_rook(b, square);
                break;
            case WHITE_QUEEN:
                material_white += QUEEN_VALUE;
                score_white += evaluate_queen(b, square);
                break;
            case WHITE_KING:
                break;
            case BLACK_PAWN:
                material_black += PAWN_VALUE;
                score_black += evaluate_pawn(b, square);
                break;
            case BLACK_KNIGHT:
                material_black += KNIGHT_VALUE;
                score_black += evaluate_knight(b, square);
                break;
            case BLACK_BISHOP:
                material_black += BISHOP_VALUE;
                score_black += evaluate_bishop(b, square);
                break;
            case BLACK_ROOK:
                material_black += ROOK_VALUE;
                score_black += evaluate_rook(b, square);
                break;
            case BLACK_QUEEN:
                material_black += QUEEN_VALUE;
                score_black += evaluate_queen(b, square);
                break;
            case BLACK_KING:
                break;
        }
    }

    // mate level?
    if (material_white <= MATE_SEARCH_LEVEL || material_black <= MATE_SEARCH_LEVEL) {
        MoveGenerator g(b);
        if (g.check_for_check_simple()) {
            //if player to move is white.. then black just made a check move.
            if (b->to_move == WHITE) {
                score_black += BONUS_CHECK;
            } else {
                score_white += BONUS_CHECK;
            }
        }
    }

    // evaluate kings
    score_white += evaluate_king(b, b->white_king, material_white, material_black);
    score_black += evaluate_king(b, b->black_king, material_white, material_black);

    // special moves
    // TODO: not all implemented

    // Castling bonus
    if (b->history.back().m.special == MOVE_CASTLE_LONG
            || b->history.back().m.special == MOVE_CASTLE_SHORT) {

        //if player to move is white then black just made a castling move
        if (b->to_move == WHITE) {
            score_black += BONUS_CASTLING;
        } else {
            score_white += BONUS_CASTLING;
        }
    }

    score_white += material_white;
    score_black += material_black;

    // final score is relative to the side to move
    return b->to_move * (score_white - score_black);
}
