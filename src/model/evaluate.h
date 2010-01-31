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

#ifndef EVALUATE_H_
#define EVALUATE_H_

#include "../common/define.h"
#include "Board.h"
#include "MoveGenerator.h"

extern int evaluate(Board* board);
extern int evaluate_pawn(Board* b, int sq);
extern int evaluate_knight(Board* b, int sq);
extern int evaluate_bishop(Board* b, int sq);
extern int evaluate_rook(Board* b, int sq);
extern int evaluate_queen(Board* b, int sq);
extern int evaluate_king(Board* b, int sq, int material_white, int material_black);
extern int repetitions(Board* b);

#endif /* EVALUATE_H_ */
