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

#include "XBoard.h"

XBoard::XBoard() : game_started(false), version(0) {
    cout.setf(ios::unitbuf);
    cerr.setf(ios::unitbuf);
}

XBoard::~XBoard() {
    end_game();
}

void XBoard::start() {
    string line = "";
    vector<string> options;
    int command = 0;
    while (command != XB_QUIT) {
        if (getline(cin, line)) {
            options.clear();
            command = xboard_command(line, options);
            switch (command) {
                case XB_XBOARD:
                case XB_ACCEPTED:
                case XB_RANDOM:
                case XB_TIME:
                case XB_OTIM:
                case XB_POST:
                case XB_NOPOST:
                case XB_FORCE:
                case XB_HARD:
                case XB_COMPUTER:
                case XB_LEVEL:
                case XB_MOVE_NOW:
                    break;
                case XB_NEW:
                    new_game();
                    break;
                case XB_RESULT:
                    end_game();
                    break;
                case XB_PROTOVER:
                    version = atoi(options[0].c_str());
                    cerr << "can: version " << version << endl;
                    //name
                    cout << "feature myname=\"" << PROJECT_NAME;
                    cout << " " << VERSION << "\" ";
                    //sigint off
                    cout << " sigint=0 ";
                    //sigterm off
                    cout << " sigterm=0 ";
                    //colors off
                    cout << " colors=0 ";
                    cout << " done=1" << endl;
                    break;
                case XB_MOVE:
                    xboard_moved();
                    break;
                case XB_GO:
                    computer_move();
                    break;
                case XB_REMOVE:
                    remove_move();
                    break;
                case XB_UNDO:
                    undo_move();
                    break;
                case XB_ILLEGAL_MOVE:
                    cout << "Illegal move" << endl;
                    break;
                case XB_UNKNOWN_COMMAND:
                    cout << "Error (unkown command): " << line << endl;
                    break;
                case XB_QUIT:
                    end_game();
                    break;
                default:
                    cerr << "Default: " << command << ": " << line << endl;
            }
        }
    }
}

int XBoard::xboard_command(const string& line, vector<string>& args) {
    split(line, args, ' ');

    if (args.size() > 0) {
        if (args[0] == "xboard") {
            return XB_XBOARD;
        }
        if (args[0] == "protover") {
            args.erase(args.begin());
            return XB_PROTOVER;
        }
        if (args[0] == "accepted") {
            return XB_ACCEPTED;
        }
        if (args[0] == "new") {
            return XB_NEW;
        }
        if (args[0] == "force") {
            return XB_FORCE;
        }
        if (args[0] == "go") {
            return XB_GO;
        }
        if (args[0] == "remove") {
            return XB_REMOVE;
        }
        if (args[0] == "undo") {
            return XB_UNDO;
        }
        if (args[0] == "result") {
            return XB_RESULT;
        }
        if (args[0] == "?") {
            return XB_MOVE_NOW;
        }
        if (args[0] == "random") {
            return XB_RANDOM;
        }
        if (args[0] == "level") {
            return XB_LEVEL;
        }
        if (args[0] == "hard") {
            return XB_HARD;
        }
        if (args[0] == "computer") {
            return XB_COMPUTER;
        }
        if (args[0] == "time") {
            return XB_TIME;
        }
        if (args[0] == "otim") {
            return XB_OTIM;
        }
        if (args[0] == "post") {
            return XB_POST;
        }
        if (args[0] == "nopost") {
            return XB_NOPOST;
        }
        if (args[0] == "quit") {
            end_game();
            return XB_QUIT;
        }


        //if you reached at this point.. probably there is a move
        if (args.size() == 1 && (args[0].size() == 4 || args[0].size() == 5)) {
            if (legal_move(args[0])) {
                return XB_MOVE;
            }
            return XB_ILLEGAL_MOVE;
        }
    }
    return XB_UNKNOWN_COMMAND;
}

void XBoard::new_game() {
    if (game_started) {
        end_game();
    }
    string fen = DEFAULT_FEN;
    board = new Board(fen);
    player = new ComputerPlayer();
    player->set_xboard(true);
    player->set_board(board);
    game_started = true;
}

void XBoard::end_game() {
    if (game_started) {
        delete board;
        delete player;
    }
    game_started = false;
}

void XBoard::xboard_moved() {
    int status;

    board->add_pgn(move_to_algebraic(xboard_move, *board));
    board->play_move(xboard_move);

    status = update_board_status(board);
    board -> set_status(status);

    if (pgn_game_result(status) != "*") {
        cout << pgn_game_result(status) << " {Game over}" << endl;
    } else {
        computer_move();
    }
}

void XBoard::computer_move() {
    int status;
    move m;
    m = player->get_move();
    board->add_pgn(move_to_algebraic(m, *board));
    board->play_move(m);
    cout << "move " << move_to_string_basic(m) << endl;
    status = update_board_status(board);
    board -> set_status(status);
    if (pgn_game_result(status) != "*") {
        cout << pgn_game_result(status) << " {Game over}" << endl;
    }
}

void XBoard::remove_move() {
    board->undo_move();
    board->undo_move();
}

void XBoard::undo_move() {
    board->undo_move();
}

bool XBoard::legal_move(const string& input) {
    if (game_started) {
        MoveGenerator generator(board);
        generator.generate_all_moves();
        for (unsigned i = 0; i < generator.get_all_moves().size(); ++i) {
            if (input == move_to_string_basic(generator.get_all_moves()[i])) {
                xboard_move = generator.get_all_moves()[i];
                return true;
            }
        }
    }
    return false;
}

