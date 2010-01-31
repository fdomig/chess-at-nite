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

#ifndef COMPUTERPLAYER_H_
#define COMPUTERPLAYER_H_

#include <memory.h>
#include <time.h>
#include "Player.h"
#include "../model/OpeningBook.h"
#include "../model/evaluate.h"

class ComputerPlayer : public Player {
public:
    ComputerPlayer(bool use_opening_book = true);
    move get_move();
private:
    bool use_opening_book;
    OpeningBook opening_book;
    move search_pv();
    int alpha_beta(int depth, int alpha, int beta);
    int quiescence(int alpha, int beta);
    void sort_pv(vector<move>& moves);
};

#endif /* COMPUTERPLAYER_H_ */
