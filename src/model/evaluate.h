/*
 * ChessAtNite
 *
 * Written by:
 *   Franziskus Domig
 *   Panayiotis Lipiridis
 *   Radoslav Petrik
 *   Thai Gia Tuong
 *
 * Chess implementation for AI course during 5th semester at IHK
 *
 */

#ifndef EVALUATE_H_
#define EVALUATE_H_

#include "../common/define.h"
#include "Board.h"
#include "MoveGenerator.h"

extern int evaluate(Board* board);
extern inline int evaluate_pawn(Board* b, int sq);
extern inline int evaluate_knight(Board* b, int sq);
extern inline int evaluate_bishop(Board* b, int sq);
extern inline int evaluate_rook(Board* b, int sq);
extern inline int evaluate_queen(Board* b, int sq);
extern inline int evaluate_king(Board* b, int sq, int material_white, int material_black);
extern int repetitions(Board* b);

#endif /* EVALUATE_H_ */
