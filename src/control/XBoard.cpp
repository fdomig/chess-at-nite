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

XBoard::XBoard() : game_started(false), version(0), show_thinking(false) {
    cout.setf(ios::unitbuf);
    cerr.setf(ios::unitbuf);
}

XBoard::~XBoard() {
    end_game();
}

bool XBoard::start() {
    cout << "# " << PROJECT_NAME << " " << VERSION << ", (c) 2009-2010\n";
    cout << "# The default mode is using xboard commands for graphical user interface.\n";
#ifndef WIN32
    cout << "#    Download and run XBoard using this command:\n";
    cout << "#    xboard -fcp \"./chess-at-nite\" -fd \"/path/to/chess-at-nite/bin/\"\n";
#else
    cout << "#    Download and run WinBoard by providing \"chess-at-nite.exe\" as an engine.\n";
#endif
    cout << "# If you want to play chess using the command line interface type \"cli\".\n";

    string line = "";
    string fen;
    vector<string> options;
    int command = 0;
    while (command != XB_QUIT && command != ABORT) {
        if (getline(cin, line)) {
            options.clear();
            command = xboard_command(line, options);
            switch (command) {
                case XB_XBOARD:
                case XB_ACCEPTED:
                case XB_RANDOM:
                case XB_TIME:
                case XB_OTIM:
                case XB_HARD:
                case XB_COMPUTER:
                case XB_LEVEL:
                case XB_MOVE_NOW:
                    break;
                case XB_NEW:
                    new_game();
                    break;
                case XB_SETBOARD:
                    fen = options[0];
                    for (unsigned index = 1; index < options.size(); ++index) {
                        fen.append(" ");
                        fen.append(options[index]);
                    }
                    new_game(fen);
                    break;
                case XB_RESULT:
                    end_game();
                    break;
                case XB_PROTOVER:
                    version = atoi(options[0].c_str());
                    //name
                    cout << "feature myname=\"" << PROJECT_NAME;
                    cout << " " << VERSION << "\" ";
                    //features on
                    cout << " variants=\"normal\" ";
                    cout << " usermove=1 "; //send usermove before the move
                    cout << " san=1 "; //use algebraic notations for moves
                    cout << " setboard=1 "; //setting the board using FEN
                    cout << " ping=1 ";

                    //features off
                    cout << " time=0 ";
                    cout << " sigint=0 ";
                    cout << " sigterm=0 ";
                    cout << " colors=0 ";
                    cout << " draw=0 ";
                    cout << " analyze=0 ";
                    cout << " ics=0 ";
                    cout << " done=1" << endl;
                    break;
                case XB_PING:
                    cout << "pong " << options[0] << endl;
                    break;
                case XB_POST:
                    show_thinking = true;
                    break;
                case XB_NOPOST:
                    show_thinking = false;
                    break;
                case XB_USERMOVE:
                    xboard_moved();
                    break;
                case XB_FORCE:
                    force_mode = true;
                    break;
                case XB_GO:
                    computer_move();
                    force_mode = false;
                    break;
                case XB_REMOVE:
                    remove_move();
                    break;
                case XB_UNDO:
                    undo_move();
                    break;
                case XB_ILLEGAL_MOVE:
                    cout << "Illegal move";
                    if (board->get_status() == STATUS_CHECK) {
                        cout << " (in check)";
                    }
                    cout << ": " << options[0] << endl;
                    break;
                case XB_UNKNOWN_COMMAND:
                    cout << "Error (unkown command): " << line << endl;
                    break;
                case XB_QUIT:
                    end_game();
                    break;
                case ABORT:
                    break;
                default:
                    cerr << "#not handled: " << command << ": " << line << endl;
            }
        }
    }

    return command == XB_QUIT;
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
        if (args[0] == "setboard") {
            args.erase(args.begin());
            return XB_SETBOARD;
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
        if (args[0] == "ping") {
            args.erase(args.begin());
            return XB_PING;
        }
        if (args[0] == "quit") {
            return XB_QUIT;
        }
        if (args[0] == "cli") {
            return ABORT;
        }
        if (args[0] == "usermove") {
            args.erase(args.begin());
            xboard_move = algebraic_to_move(args[0], *board);
            if (xboard_move.special != MOVE_ERROR) {
                return XB_USERMOVE;
            }
            return XB_ILLEGAL_MOVE;
        }
    }
    return XB_UNKNOWN_COMMAND;
}

void XBoard::new_game(const string& fen) {
    if (game_started) {
        end_game();
    }
    board = new Board(fen);
    player = new ComputerPlayer();
    player->set_xboard(true);
    player->set_board(board);
    game_started = true;
    force_mode = false;
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
        cout << pgn_game_result(status);
        cout << " {" << pgn_game_result_comment(status) << "}" << endl;
    } else if (!force_mode) {
        computer_move();
    }
}

void XBoard::computer_move() {
    int status;
    move m;
    player->set_show_thinking(show_thinking);
    m = player->get_move();
    //do not change the order.. otherwise the algebraic notation will be wrong
    cout << "move " << move_to_algebraic(m, *board) << endl;
    board->play_move(m);

    status = update_board_status(board);
    if (pgn_game_result(status) != "*") {
        cout << pgn_game_result(status);
        cout << " {" << pgn_game_result_comment(status) << "}" << endl;
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

