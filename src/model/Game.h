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

#ifndef GAME_H_
#define GAME_H_

#include <string>
#include <iostream>
#include <string>
#include "MoveGenerator.h"
#include "Board.h"
#include "evaluate.h"
#include "../player/Player.h"
#include "../common/extra_utils.h"

class Player;

class Game {
public:
    Game(Board* board, Player* white_player, Player* black_player, 
            bool both_human = false);

    Game(const Game& game);
    virtual ~Game();
    void start_game();
private:
    Board* board;
    Player* white_player;
    Player* black_player;
    MoveGenerator* move_generator;

    bool both_human;
    bool game_over;
    void play(Player* player1, Player* player2);
};

#endif /* GAME_H_ */
