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

#ifndef _EXTRA_UTILS_H
#define	_EXTRA_UTILS_H

#include <iostream>
#include <fstream>
#include <time.h>
#include "define.h"
#include "../model/Board.h"
#include "../model/MoveGenerator.h"
#include "../model/evaluate.h"
#include "utils.h"

extern int board_status(const Board& board);
extern int update_board_status(Board* board);
extern string move_to_algebraic_simple(const move& m, const vector<move>& moves);
extern string move_to_algebraic(const move& m, const Board& board);
extern void print_algebraic_moves(const vector<move>& moves, const Board& board);
extern string strip_algebraic(const string& algebraic);
extern move algebraic_to_move(const string& algebraic, const Board& board);
extern string pgn_game_result(int status);
extern string pgn_game_result_comment(int status);
extern bool write_last_game_pgn(const string& file_name, Board* board,
        const string& white, const string& black);
extern void print_history_debug(Board* board);

#endif	/* _EXTRA_UTILS_H */

