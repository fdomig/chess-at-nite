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

#ifndef CLI_H_
#define CLI_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include "../model/Game.h"
#include "../model/Board.h"
#include "../player/ComputerPlayer.h"
#include "../player/HumanPlayer.h"

class Player;
class Game;
class Board;

#define QUIT            0
#define HUMAN_VS_CPU    1
#define CPU_VS_HUMAN    2
#define HUMAN_VS_HUMAN  3
#define CPU_VS_CPU      4
#define SELECT_FEN      5
#define SHOW_HELP       6
#define BENCHMARK       7
#define WAC             8

#define BENCHMARK_FEN "r3r1k1/pp3pbp/1qp1b1p1/2B5/2BP4/Q1n2N2/P4PPP/3R1K1R w - - 0 18"

class CLI {
public:
    CLI();
    CLI(int option);
    virtual ~CLI();

    void start();

private:
    void init();
    Player* white_player;
    Player* black_player;
    Board* board;
    Game* game;
    //when the only human player is black.. 
    bool rotated_board;
    std::string fen;

    void show_options();
    int get_user_option();
    void init_game(int game_type);
    void start_game();
    void end_game();
    void show_about();
    void select_fen();
    void run_benchmark();
    void run_wac_test();
    void apply_option(int option);
    std::string get_line();
};

#endif /* CLI_H_ */
