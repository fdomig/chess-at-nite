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
