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

#include "ComputerPlayer.h"

ComputerPlayer::ComputerPlayer(bool use_book) :
Player(), use_opening_book(use_book) {
    name = PROJECT_NAME;
    name.append(" ");
    name.append(VERSION);
    cout.setf(ios::unitbuf);
}

move ComputerPlayer::get_move() {
    move m;

#ifdef USE_OPENING_BOOK
    if (use_opening_book && opening_book.is_opened()) {
        MoveGenerator generator(board);
        generator.generate_all_moves();
        vector<move>& moves = generator.get_all_moves();
        if (opening_book.get_move(moves, board->history, m)) {
            if (!xboard) {
                cout << " from book: " << move_to_algebraic(m, *board) << endl;
            }
            return m;
        }
    }
#endif

    if (!xboard) {
        cout << " thinking... " << endl;
    }
    m = search_pv();
    return m;
}

move ComputerPlayer::search_pv() {
    int start_time = get_ms();
    //time is in seconds
    board->stop_time = start_time + max_thinking_time * 1000;
    board->time_exit = false;

    board->ply = 0;
    board->checked_nodes = 0;

    memset(board->pv, 0, sizeof (board->pv));

    if (!xboard && show_thinking) {
        cout << "ply  score   time   nodes  pv\n";
    }

    move best_moves[MAX_SEARCH_DEPTH];
    int best_scores[MAX_SEARCH_DEPTH];
    int best_moves_plys[MAX_SEARCH_DEPTH];

    int depth = 1;
    bool found_checkmate = false;
    for (; depth <= max_search_depth && !board->time_exit; depth++) {
        board->follow_pv = true;
        int score = alpha_beta(depth, -EVALUATION_START, EVALUATION_START);
        best_moves[depth] = board->pv[0][0];
        best_moves_plys[depth] = board->pv_length[0];
        best_scores[depth] = score;

        if (show_thinking) {
            if (!board->time_exit) {
                if (xboard) {
                    //ply score time nodes pv
                    int centiseconds = (int) ((double) (get_ms() - start_time) / 10);
                    cout << setw(3) << depth;
                    cout << setw(7) << score << " ";
                    cout << setw(5) << centiseconds << " ";
                    cout << setw(8) << board->checked_nodes << " ";
                } else {
                    printf("%3d %6s %6s %7s  ", depth,
                            display_score(score),
                            display_time(start_time, get_ms()),
                            display_nodes_count(board->checked_nodes));
                }
                //you have to simulate the game to print the algebraic correct
                Board temp_board = Board(*board);
                for (int j = 0; j < board->pv_length[0]; ++j) {
                    if (board->to_move == BLACK) {
                        if (j == 0) {
                            cout << board->full_moves << ". ... ";
                        } else {
                            if ((j + 1) % 2 == 0) {
                                cout << board->full_moves + (j / 2 + 1) << ". ";
                            }
                        }
                    } else {
                        if (j % 2 == 0) {
                            cout << board->full_moves + (j / 2) << ". ";
                        }
                    }
                    cout << move_to_algebraic(board->pv[0][j], temp_board) << " ";
                    temp_board.play_move(board->pv[0][j]);
                }
                cout << endl;
            }
        }
        if (abs(score) >= MATE) {
            found_checkmate = true;
            break;
        }
    }

    move best_move;
    int best_move_plys;
    int best_score;

    if (board->time_exit) {
        best_move = best_moves[depth - 2];
        best_move_plys = best_moves_plys[depth - 2];
        best_score = best_scores[depth - 2];
    } else if (found_checkmate) {
        best_move = best_moves[depth];
        best_move_plys = best_moves_plys[depth];
        best_score = best_scores[depth];
    } else {
        best_move = best_moves[depth - 1];
        best_move_plys = best_moves_plys[depth - 1];
        best_score = best_scores[depth - 1];
    }

#ifdef SHOW_SEARCH_INFO
    if (!xboard) {
        float total_time = (float) (get_ms() - start_time) / 1000;
        printf("%s nodes searched in %.2f secs (%.1fK nodes/sec)\n",
                display_nodes_count(board->checked_nodes), total_time,
                (board->checked_nodes / 1000.0) / total_time);
    }
#endif

    if (!xboard && show_best_score) {
        cout << "Score for move " << move_to_algebraic(best_move, *board) << " is ";
        cout << display_score(best_score) << " (" << best_move_plys << " plys)";
        cout << endl;
    }
    return best_move;
}

int ComputerPlayer::alpha_beta(int depth, int alpha, int beta) {

    if (depth == 0) {
        return quiescence(alpha, beta);
    }
    board->checked_nodes++;

    // check the time every 4096 nodes
    if ((board->checked_nodes & 4095) == 0) {
        if (get_ms() > board->stop_time) {
            board->time_exit = true;
            return 0;
        }
    }

#ifdef USE_HASH_TABLE
    // EXPERIMENTAL: check for a hash entry
    switch (board->hash_probe(depth, &alpha, beta)) {
        case EXACT:
            if (alpha < beta) {

                board->pv_length[board->ply] = board->ply;
                htentry* entry = board->hash_entry(board->get_hash());
                board->pv[board->ply][board->ply] = entry->best;

                for (int j = board->ply + 1; j < board->pv_length[board->ply
                        + 1]; ++j) {
                    board->pv[board->ply][j] = board->pv[board->ply + 1][j];
                }
                board->pv_length[board->ply] = board->pv_length[board->ply + 1];

            }
            return alpha;
        case LOWER:
            return beta;
        case UPPER:
            return alpha;
        case NO:
            break;
    }
#endif // USE_HASH_TABLE
    board->pv_length[board->ply] = board->ply;

    MoveGenerator generator(board);
    generator.generate_all_moves();
    vector<move>& moves = generator.get_all_moves();

    // are we in check? so we search deeper
    bool check = generator.king_under_check;
    if (check) {
        depth++;
    }

    // If this is a root node, we can't just return 0, we need a move.
    // else we check for a repetition and assume that this is a draw.
    if (board->ply > 0 && repetitions(board)) {
        return DRAW;
    }

    //TODO: maybe it's better to use the get_all_moves(move best_move) from
    // move generator, cause this function puts the best move in front without
    // changing of the moves.. don't forget that they are sorted
    if (board->follow_pv) {
        sort_pv(moves);
    }

    bool played_move = false;
    int score = 0;
    bool pv_search = true;

#ifdef USE_HASH_TABLE
    int o_alpha = alpha;
    move best;
#endif

    for (unsigned index = 0; index < moves.size(); index++) {
        board->fake_move(moves[index]);
        played_move = true;
        if (pv_search) {
            score = -alpha_beta(depth - 1, -beta, -alpha);
        } else {
            score = -alpha_beta(depth - 1, -alpha - 1, -alpha);
            if (score > alpha && score < beta) {
                // re-search
                score = -alpha_beta(depth - 1, -beta, -alpha);
            }
        }
        board->unfake_move();

        // we have no time left
        if (board->time_exit) {
            return 0;
        }

        if (score > alpha) {
            if (score >= beta) {

#ifdef USE_HASH_TABLE
                board->hash_store(depth, LOWER, score, moves[index]);
#endif // USE_HASH_TABLE
                return score;
            }
            alpha = score;

#ifdef USE_HASH_TABLE
            best = moves[index];
#endif
            pv_search = false;

            // store the new, better alpha node in the path
            board->pv[board->ply][board->ply] = moves[index];
            for (int j = board->ply + 1; j < board->pv_length[board->ply + 1]; ++j) {
                board->pv[board->ply][j] = board->pv[board->ply + 1][j];
            }
            board->pv_length[board->ply] = board->pv_length[board->ply + 1];
        }
    }

    // if we didn't played a move, we are either checkmate or stalemate
    if (!played_move) {
        if (check) {
            return -(MATE + board->ply);
        }
        return DRAW;
    }

    if (board->fifty_moves >= FIFTY_MOVES_RULE) {
        return DRAW;
    }
#ifdef USE_HASH_TABLE
    htype type = (alpha == o_alpha) ? UPPER : EXACT;
    if (alpha != o_alpha) {
        // TODO: change PV
    }
    board->hash_store(depth, type, alpha, best);
#endif // USE_HASH_TABLE
    return alpha;
}

int ComputerPlayer::quiescence(int alpha, int beta) {

    board->checked_nodes++;

    // check the time every 4096 nodes
    if ((board->checked_nodes & 4095) == 0) {
        if (get_ms() > board->stop_time) {
            board->time_exit = true;
            return 0;
        }
    }

    board->pv_length[board->ply] = board->ply;

    // check with the evaluation function
    int e = evaluate(board);
    if (e >= beta) {
        return beta;
    }
    if (e > alpha) {
        alpha = e;
    }

    MoveGenerator generator(board);
    generator.generate_all_moves();
    vector<move> moves;
    generator.get_all_capture_moves(moves);

    for (vector<move>::const_iterator it = moves.begin(); it != moves.end(); ++it) {
        board->fake_move(*it);
        int score = -quiescence(-beta, -alpha);
        board->unfake_move();
        if (score > alpha) {
            if (score >= beta) {
                return beta;
            }
            alpha = score;

            // store the new, better alpha node in the path
            board->pv[board->ply][board->ply] = *it;
            for (int j = board->ply + 1; j < board->pv_length[board->ply + 1]; ++j) {
                board->pv[board->ply][j] = board->pv[board->ply + 1][j];
            }
            board->pv_length[board->ply] = board->pv_length[board->ply + 1];
        }
    }
    return alpha;
}

void ComputerPlayer::sort_pv(vector<move>& moves) {
    board->follow_pv = false;
    for (unsigned i = 0; i < moves.size(); i++) {
        if (moves[i] == board->pv[0][board->ply]) {
            board->follow_pv = true;
            move tmp = moves[0];
            moves[0] = moves[i];
            moves[i] = tmp;
            return;
        }
    }
}
