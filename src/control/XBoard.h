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

#ifndef _XBOARD_H
#define	_XBOARD_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "../common/utils.h"
#include "../model/Board.h"
#include "../player/ComputerPlayer.h"
#include "../model/MoveGenerator.h"


#define XB_XBOARD     10
#define XB_PROTOVER   20
#define XB_ACCEPTED   21
#define XB_PING       22
#define XB_NEW        30
#define XB_RANDOM     31
#define XB_LEVEL      32
#define XB_HARD       33
#define XB_COMPUTER   34
#define XB_SETBOARD   35


#define XB_USERMOVE   40
#define XB_REMOVE     41
#define XB_UNDO       42
#define XB_RESULT     44
#define XB_MOVE_NOW   49
#define XB_FORCE      50
#define XB_GO         51
#define XB_TIME       70
#define XB_OTIM       71
#define XB_POST       72
#define XB_NOPOST     73

#define XB_ERROR             -1
#define XB_UNKNOWN_COMMAND   -2
#define XB_ILLEGAL_MOVE      -3
#define XB_QUIT            1000
#define ABORT              2000


using namespace std;

class XBoard {
public:
    XBoard();
    virtual ~XBoard();
    bool start();
private:
    bool game_started;
    int version;
    ComputerPlayer* player;
    Board* board;
    move xboard_move;

    bool show_thinking;
    bool force_mode;
    bool legal_move(const string& input);

    int xboard_command(const string& line, vector<string>& args);
    void new_game(const string& fen=DEFAULT_FEN);
    void end_game();
    void xboard_moved();
    void computer_move();
    void remove_move();
    void undo_move();
};

#endif	/* _XBOARD_H */

