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

#ifndef CLI_H_
#define CLI_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include "../model/Game.h"
#include "../model/Board.h"
#include "../model/MoveGenerator.h"
#include "../player/ComputerPlayer.h"
#include "../player/HumanPlayer.h"
#include "../common/utils.h"
#include "PGN.h"

class Player;
class Game;
class Board;

#define QUIT            0
#define HUMAN_VS_CPU    1
#define CPU_VS_HUMAN    2
#define HUMAN_VS_HUMAN  3
#define CPU_VS_CPU      4
#define LOAD            5
#define SHOW_HELP       6
#define BENCHMARK       7
#define WAC             8

#define SETTINGS        9

//setings defines
#define SET_MAX_TIME         1
#define SET_MAX_DEPTH        2
#define SET_SHOW_BEST_SCORE  3
#define SET_SHOW_THINKING    4

//loading defines
#define LOAD_NEW_GAME        1
#define LOAD_LAST_GAME       2
#define LOAD_FROM_PGN        3
#define LOAD_FROM_FEN        4

class CLI {
public:
    CLI();
    virtual ~CLI();
    void start(int option=-1);

private:
    void init();
    Player* white_player;
    Player* black_player;
    Board* board;
    Game* game;
    PGN pgn;
    string message;
    //when the only human player is black.. 
    bool inverse_board;
    bool both_human;
    //time is in seconds
    int max_thinking_time;
    int max_search_depth;
    bool show_best_score;
    bool show_thinking;
    bool loaded_game;
    std::string fen;


    void show_about();
    void show_options();
    void print_messages();
    void apply_option(int option);
    void show_settings();
    void read_settings();
    void apply_settings(int option);
    void show_load();
    void read_load();
    void apply_load(int option);
    void set_max_time_from_user();
    void set_max_depth_from_user();
    int get_user_option();
    void init_game(int game_type);
    void start_game();
    void end_game();
    void read_fen();
    void run_benchmark();
    void run_wac_test();
    std::string get_line();

    bool compare_found_move(string found, string should);
};

#endif /* CLI_H_ */
