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

#ifndef PLAYER_H_
#define PLAYER_H_

#include "../common/define.h"
#include "../common/utils.h"
#include "../model/Board.h"
#include "../model/MoveGenerator.h"
#include "../common/extra_utils.h"

class Player {
public:
    Player();
    virtual ~Player();
    void set_board(Board* board);
    virtual move get_move() = 0;
    void set_max_thinking_time(int seconds);
    void set_max_search_depth(int depth);
    void set_show_best_score(bool show);
    void set_show_thinking(bool show);
    void set_xboard(bool xboard);
    string& get_name();
protected:
    Board* board;
    string name;
    int max_thinking_time;
    int max_search_depth;

    bool show_best_score;
    bool show_thinking;
    bool xboard;
};

#endif // PLAYER_H_
