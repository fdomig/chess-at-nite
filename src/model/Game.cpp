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

Game::Game(Board* board, Player* new_white_player, Player* new_black_player,
        bool both_human) :
board(board), white_player(new_white_player), black_player(new_black_player),
both_human(both_human) {
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
    string color;
    int status;
    bool success = true;
    while (!game_over) {
        color = board -> to_move == WHITE ? "White" : "Black";
        status = update_board_status(board);

        board -> set_status(status);

        //if the move was successfull or the undo was valid then print.
        if (success) {
            cout << *board << endl;
        }

        //what to do in different cases
        switch (status) {
            case STATUS_CHECKMATE:
            case STATUS_STALEMATE:
                game_over = true;
                break;
        }

        if (game_over) {
            break;
        }

        //TODO: should leave from here...
        if (repetitions(board) >= 2) {
            cout << "You could claim a draw...!" << endl;
        }

        cout << color << "'s move " << "[" << board->number_of_moves / 2 + 1 << "]...";
        move next_move = player1->get_move();

        switch (next_move.special) {
            case MOVE_UNDO:
                success = board->undo_move();
                //that means that you have to undo 2 moves in order to give
                //the chance for the human player to move again..
                if (!both_human) {
                    success = board->undo_move();
                }

                if (success) {
                    cout << *board << endl;
                } else {
                    cout << "There is nothing in the history to undo.." << endl;
                }
                continue;
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

            board->add_pgn(move_to_algebraic(next_move, *board));
            board->play_move(next_move);
            success = true;
            play(player2, player1);
        }
    }
}
