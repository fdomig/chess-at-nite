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

#include "common/globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include "common/define.h"
#include "model/MoveGenerator.h"
#include "model/Game.h"
#include "player/Player.h"
#include "player/HumanPlayer.h"
#include "player/ComputerPlayer.h"
#include "control/CLI.h"
#include "common/extra_utils.h"
#include "control/PGN.h"
#include "control/XBoard.h"

void test();

int main(int argc, char **argv) {
#ifdef DEBUG
    cerr << "!!!!!!!!!!!!!!!!!!!!!!!!\n";
    cerr << "!!! DEBUG MODE IS ON !!!\n";
    cerr << "!!!!!!!!!!!!!!!!!!!!!!!!\n";
#endif
    srand(time(NULL));
    init_globals();
    bool cli_mode = false;
    int user_option = 0;
    if (argc > 1) {
        string tmp(argv[1]);
        if (tmp == "cli") {
            cli_mode = true;
        } else {
            user_option = atoi(argv[1]);
        }
    }

#ifdef COMMAND_LINE
    if (cli_mode) {
        CLI cli;
        cli.start();
    } else if (user_option > 0) {
        CLI cli;
        cli.start(user_option);
    } else {
        XBoard xboard;
        if (!xboard.start()) {
            CLI cli;
            cli.start();
        }
    }
#else
    // for testing
    test();
#endif
    return 0;
}

/*
 * For testing purposes during developing.
 */
void test() {
    string default_board = DEFAULT_FEN;

    default_board = "5q2/k113PP/2Q5/8/2K4r/3P4/4P3/8 w - - 0 20";
    default_board = "8/4p3/k3p3/4P3/8/K7/8/8 w - - 0 100";
    default_board = "bn6/8/k3p3/4P3/8/K7/8/BN6 w - - 0 100";

    //Bishop and knight checkmate (wikipedia)
    //white to move will checkmate black!
    default_board = "7k/8/5K2/4N3/8/3B4/8/8 w - - 0 1";
    //Deletang's second triangle
    default_board = "3k4/8/4K3/1B1N4/8/8/8/8 w - - 0 1";

    //two Queens vs a Queen..
    default_board = "6k1/5qq1/8/8/8/8/Q7/4K3 w - - 0 1";

    default_board = DEFAULT_FEN;

    Board board = Board(default_board);

    Player* white_player = new HumanPlayer();
    Player* black_player = new ComputerPlayer();

    Game game = Game(&board, white_player, black_player);
    game.start_game();

    delete white_player;
    delete black_player;
}
