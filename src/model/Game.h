/*
 * This file is part of the chess-at-nite project.
 *
 * Copyright (c) 2009-2010 by
 *   Franziskus Domig
 *   Panayiotis Lipiridis
 *   Radoslav Petrik
 *   Thai Gia Tuong
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
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

class Player;

class Game {
public:
    Game(Board* board, Player* white_player, Player* black_player, int max_moves = 100000);
    virtual ~Game();
    void init();
    void start_game();
    void play(Player* player1, Player* player2);
private:
    Board* board;
    Player* white_player;
    Player* black_player;
    MoveGenerator* move_generator;
    int max_moves;
    bool game_over;
};

#endif /* GAME_H_ */
