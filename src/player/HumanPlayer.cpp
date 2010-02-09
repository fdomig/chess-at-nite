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

#include "HumanPlayer.h"

using namespace std;

HumanPlayer::HumanPlayer(): Player() {
    name = "Human Player";
}

move HumanPlayer::get_move() {
    move m = get_valid_move_from_user();
    cout << "";
    return m;
}

/*
 * Asking from user for a move in the format: A1B1
 *
 * For the promotions.. the 5th char will be the promoted piece! (a7a8q = promotion to queen)
 *
 * it will return m.special = MOVE_ERROR if something went wrong..
 * otherwise the next guy should try to apply that move on the actual board and check if it's legal
 * of course taking into consideration who is about to play, if it's a castling, promotion etc...
 */
move HumanPlayer::get_move_from_user() {
    string input;
    cin.clear();
    cin >> input;
    move m = algebraic_to_move(input, *board);

    if (m.special == MOVE_ERROR) {
        m = string_to_move(input);
    }
    return m;
}

move HumanPlayer::get_valid_move_from_user() {
    move m;
    MoveGenerator move_generator(board);

    move_generator.generate_all_moves();
    vector<move>& moves = move_generator.get_all_moves();

    bool found = false;
    while (!found) {
        found = false;
        if (board->get_status() == STATUS_CHECK) {
            cout << "Enter your move (Check!): ";
        } else {
            cout << "Enter your move: ";
        }

        m = get_move_from_user();
        switch (m.special) {
            case MOVE_ERROR:
                cerr << "Wrong input.. try /? for help" << endl;
                continue;
            case MOVE_UNDO:
                found = true;
                break;
            case MOVE_SHOW_MOVES:
                print_moves(moves);
                continue;
            case MOVE_SHOW_ALGEBRAIC:
                print_algebraic_moves(moves, *board);
                continue;
            case MOVE_RESIGN:
                found = true;
                break;
            case MOVE_DRAW:
                cout << "Asked for draw..!!" << endl;
                continue;
            case MOVE_SHOW_HISOTRY:
                print_history(board->pgn);
                continue;
            case MOVE_SHOW_BOARD:
                cout << *board;
                continue;
            case MOVE_SHOW_HELP:
                print_help();
                continue;
            case MOVE_ORDINARY:
            case MOVE_CASTLE_SHORT:
            case MOVE_CASTLE_LONG:
            case MOVE_EN_PASSANT:
            case MOVE_PROMOTION:
                if (board->to_move * board->board[m.pos_old] > 0) {
                    m.moved_piece = board->board[m.pos_old];
                    m.content = board->board[m.pos_new];
                    found = is_legal_move(moves, m);
                }
        }
        if (!found) {
            cerr << "Your move is illegal! Try again..." << endl;
            if (board->get_status() == STATUS_CHECK) {
                cerr << "..and don't forgtet that you are in Check..!" << endl;
            }

        }
    }
    return m;
}
