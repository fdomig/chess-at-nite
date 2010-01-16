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

#include "Game.h"

using namespace std;

Game::Game(Board* board, Player* new_white_player, Player* new_black_player, int max_moves) :
board(board), white_player(new_white_player), black_player(new_black_player),
max_moves(max_moves) {
    move_generator = new MoveGenerator(board);
    white_player->set_board(board);
    black_player->set_board(board);
}

Game::~Game() {
    delete move_generator;
}

void Game::init() {

}

void Game::start_game() {
    game_over = false;
    board->number_of_moves = 0;
    if (board->to_move == WHITE) {
        play(white_player, black_player);
    } else {
        play(black_player, white_player);
    }

    int wait = get_ms() + 2400;
    while (get_ms() < wait);
    print_history(board->history);
}

/*
 * Recursive play :)
 */
void Game::play(Player* player1, Player* player2) {
    vector<move> moves;
    move_generator->generate_all_moves();
    moves = move_generator->get_all_moves();
    string color;
    while (!game_over && board->number_of_moves < max_moves) {
        color = board -> to_move == WHITE ? "White" : "Black";
        moves = move_generator->get_all_moves();

        if (moves.empty()) {
            if (move_generator->king_under_check) {
                board->set_status(STATUS_CHECKMATE);
                game_over = true;
            } else {
                board->set_status(STATUS_STALEMATE);
                game_over = true;
            }
        } else if (move_generator->king_under_check) {
            board->set_status(STATUS_CHECK);
        } else {
            board->set_status(STATUS_NORMAL);
        }

        cout << *board << endl;
        // if the game is stalemate or checkmate..
        if (game_over) {
            break;
        }

        if (repetitions(board) >= 2) {
            cout << "You could claim a draw...!" << endl;
        }

        cout << color << "'s move " << "[" << board->number_of_moves / 2 + 1 << "]...";
        move next_move = player1->get_move();

        switch (next_move.special) {
            case MOVE_UNDO:
                if (board->history.size() >= 2) {
                    board->undo_move();
                    board->undo_move();
                    cout << board << endl;
                    continue;
                } else {
                    cout << "There is nothing in the history to undo.." << endl;
                    continue;
                }
            case MOVE_RESIGN:
                cout << "Resigned... GG" << endl;
                game_over = true;
                break;
        }

        if (!game_over) {
            if (next_move.moved_piece == EMPTY) {
                cerr << "Game tried to make an empty move.. we have to stop it.. sorry!" << endl;
                game_over = true;
                break;
            }
            board->play_move(next_move);
            play(player2, player1);
        }
    }
}
