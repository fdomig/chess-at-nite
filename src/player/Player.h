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

#ifndef PLAYER_H_
#define PLAYER_H_

#include "../common/define.h"
#include "../model/Board.h"
#include "../model/MoveGenerator.h"
#include "../common/utils.h"

class Player {
protected:
    Board* board;
public:
    Player();
    virtual ~Player();
    void set_board(Board* board);
    virtual move get_move() = 0;
};

#endif // PLAYER_H_
