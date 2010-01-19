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

#include "HumanPlayer.h"

using namespace std;

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
    return algebraic_to_move(input, *board);
}

move HumanPlayer::get_valid_move_from_user() {
    move m;
    vector<move> moves;
    MoveGenerator move_generator = MoveGenerator(board);

    move_generator.generate_all_moves();
    moves = move_generator.get_all_moves();

    bool found = false;
    while (!found) {
        found = false;
        cout << "Enter your move: ";
        m = get_move_from_user();
        switch (m.special) {
        case MOVE_ERROR:
            cout << "Wrong input.. try /? for help" << endl;
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
            print_history(board->history);
            continue;
        case MOVE_SHOW_BOARD:
            cout << *board;
            continue;
        case MOVE_SHOW_HELP:
            print_help();
            continue;
            //cause from the get_move_from_user we are getting only these:
        case MOVE_ORDINARY:
        case MOVE_PROMOTION:
            if (board->to_move * board->board[m.pos_old] > 0) {
                m.moved_piece = board->board[m.pos_old];
                m.content = board->board[m.pos_new];
                found = is_legal_move(moves, m);
            }
        }

        if (!found) {
            cout << "You entered an illegal move! Try again..." << endl;
        }
    }
    return m;
}
