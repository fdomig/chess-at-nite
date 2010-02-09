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

#include "extra_utils.h"

int board_status(const Board& board) {
    Board b = Board(board);
    MoveGenerator move_generator(&b);
    move_generator.generate_all_moves();
    if (move_generator.get_all_moves().empty()) {
        if (move_generator.king_under_check) {
            return b.to_move == BLACK ? STATUS_WHITE_MATES : STATUS_BLACK_MATES;
        } else {
            return STATUS_STALEMATE;
        }
    } else if (move_generator.king_under_check) {
        return STATUS_CHECK;
    } else if (b.fifty_moves >= FIFTY_MOVES_RULE ||
            repetitions(&b) >= THREEFOLD_REPITITION_RULE) {
        return STATUS_DRAW;
    }
    return STATUS_NORMAL;
}

int update_board_status(Board* board) {
    int status = board_status(*board);
    board -> set_status(status);
    return status;
}

/**
 * This simple version is not supporting +/# for check and checkmate
 * 
 * If there is more than one piece that can move to the given square, then the
 * starting square-file has to be added; if still not enough to distinguish,
 * then the starting square-rank instead has to be added; if still no difference
 * both starting square-file and -rank has to be added.
 */
string move_to_algebraic_simple(const move& m, const vector<move> &moves) {
    string result = "";

    //castlings
    if (m.special == CASTLE_LONG) {
        return "O-O-O";
    } else if (m.special == CASTLE_SHORT) {
        return "O-O";
    }

    //moved piece
    switch (abs(m.moved_piece)) {
        case PAWN:
            break;
        case KNIGHT:
            result.append("N");
            break;
        case BISHOP:
            result.append("B");
            break;
        case ROOK:
            result.append("R");
            break;
        case QUEEN:
            result.append("Q");
            break;
        case KING:
            result.append("K");
            break;
    }

    //disambiguating moves
    //same piece and same destination, but it's not affecting pawns. This only can
    //happen while two pawns ara attacking the same piece so the file is added anyway
    if (abs(m.moved_piece) != PAWN) {
        vector<move> same;
        same.clear();
        for (unsigned int index = 0; index < moves.size(); index++) {
            if ((moves[index].pos_old != m.pos_old) &&
                    (moves[index].moved_piece == m.moved_piece) &&
                    (moves[index].pos_new == m.pos_new)) {
                same.push_back(moves[index]);
            }
        }
        int count = 0;
        //check for different FILE of departure
        for (unsigned int index = 0; index < same.size(); index++) {
            if (FILE(same[index].pos_old) == FILE(m.pos_old)) {
                count++;
            }
        }

        if (!same.empty() && count == 0) {
            char file = FILE(m.pos_old) + 'a';
            result.push_back(file);
        } else if (count > 0) {
            count = 0;
            for (unsigned int index = 0; index < same.size(); index++) {
                if (RANK(same[index].pos_old) == RANK(m.pos_old)) {
                    count++;
                }
            }
            if (!same.empty() && count == 0) {
                char rank = RANK(m.pos_old) + '1';
                result.push_back(rank);
            } else if (count > 0) {
                //square of departure
                result.append(square_to_string(m.pos_old));
            }
        }
    }
    //captures
    if (m.content != EMPTY) {
        if (abs(m.moved_piece) == PAWN) {
            char file = FILE(m.pos_old) + 'a';
            result.push_back(file);
        }
        result.append("x");
    }

    //square of destination
    result.append(square_to_string(m.pos_new));

    //promotions
    if (m.special == MOVE_PROMOTION) {
        //specifications of PGN, we might want to remove that
        result.append("=");
        switch (abs(m.promoted)) {
            case KNIGHT:
                result.append("N");
                break;
            case BISHOP:
                result.append("B");
                break;
            case ROOK:
                result.append("R");
                break;
            case QUEEN:
                result.append("Q");
                break;
        }
    }
    return result;
}

/**
 * Full algebraic notation of the move.
 *
 * Should be called before making the move on the board.
 */
string move_to_algebraic(const move& m, const Board& board) {
    //cause we don't want to messing around with the original board!!
    Board b = Board(board);
    MoveGenerator move_generator(&b);
    move_generator.generate_all_moves();

    string algebraic = move_to_algebraic_simple(m, move_generator.get_all_moves());

    b.play_move(m);
    int status = board_status(b);
    switch (status) {
        case STATUS_CHECK:
            algebraic.append("+");
            break;
        case STATUS_WHITE_MATES:
        case STATUS_BLACK_MATES:
            algebraic.append("#");
            break;
    }

    return algebraic;
}

void print_algebraic_moves(const vector<move>& moves, const Board& board) {
    if (moves.size() == 0) {
        cout << "No available moves... :(" << endl;
        return;
    }
    cout << "---------" << setw(2) << moves.size() << " available moves--------";
    unsigned int i;
    string move;
    for (i = 0; i < moves.size(); i++) {
        if ((i % 4) == 0) {
            cout << endl;
        }
        move = move_to_algebraic(moves[i], board);
        cout << move << setw(8 - move.length()) << " ";
    }

    cout << endl;
    cout << "-----------------------------------" << endl;
}

/**
 * Removes all the special characters from algebraic notation. They are not
 * affecting the uniquenes of the move.
 *
 * Case matters here.. example:
 *  bxc3 - Pawn is capturing the piece on square c3
 *  Bxc3 - Bishop is capturing the piece on square c3
 */
string strip_algebraic(const string& algebraic) {
    string stripped = "";

    for (unsigned int i = 0; i < algebraic.size(); i++) {
        if (algebraic[i] != 'x' && algebraic[i] != '+' && algebraic[i] != '#'
                && algebraic[i] != '=' && algebraic[i] != ':' && algebraic[i] != '-') {
            stripped.push_back(algebraic[i]);
        }
    }
    return stripped;
}

/*
 * Converts the input given by a user to a move. First tries to find a match
 * in all legal moves by algebraic notation and then by a simple string to move
 * converter.
 */
move algebraic_to_move(const string& algebraic, const Board& board) {
    Board b = Board(board);
    MoveGenerator move_generator(&b);
    move_generator.generate_all_moves();
    vector<move>& moves = move_generator.get_all_moves();

    for (unsigned int i = 0; i < moves.size(); i++) {
        if (strip_algebraic(algebraic).compare(
                strip_algebraic(move_to_algebraic(moves[i], board))) == 0) {
            return moves[i];
        }
    }
    move m;
    m.special = MOVE_ERROR;
    return m;
}

string pgn_game_result(int status) {
    switch (status) {
        case STATUS_DRAW:
        case STATUS_STALEMATE:
            return "1/2-1/2";
        case STATUS_WHITE_MATES:
        case STATUS_WHITE_WINS:
            return "1-0";
        case STATUS_BLACK_MATES:
        case STATUS_BLACK_WINS:
            return "0-1";
    }
    //still in progress..
    return "*";
}

string pgn_game_result_comment(int status) {
    switch (status) {
        case STATUS_DRAW:
            return "Draw";
        case STATUS_STALEMATE:
            return "Stalemate";
        case STATUS_WHITE_MATES:
            return "White mates";
        case STATUS_WHITE_WINS:
            return "Black resigned";
        case STATUS_BLACK_MATES:
            return "Black mates";
        case STATUS_BLACK_WINS:
            return "White resigned";
    }
    return "In progress";
}

bool write_last_game_pgn(const string& file_name, Board *board,
        const string& white, const string& black) {
    ofstream last_game_pgn(file_name.c_str());
    if (last_game_pgn.is_open()) {
        string result = pgn_game_result(board->get_status());
        time_t now;
        time(&now);
        struct tm *current = localtime(&now);
        last_game_pgn << "[Event \"" << PROJECT_NAME << "\"]\n";
        last_game_pgn << "[Site \"" << "?" << "\"]\n";
        last_game_pgn << "[Date \"" << (current->tm_year + 1900);
        last_game_pgn << "." << setw(2) << setfill('0') << (current->tm_mon + 1);
        last_game_pgn << "." << setw(2) << setfill('0') << current->tm_mday << "\"]\n";

        last_game_pgn << "[Round \"?\"]\n";
        last_game_pgn << "[White \"" << white << "\"]\n";
        last_game_pgn << "[Black \"" << black << "\"]\n";
        last_game_pgn << "[Result \"" << result << "\"]\n";

        last_game_pgn << "\n";
        //movetext
        string temp;
        char move_num_char[10];
        int width = 0;
        for (unsigned int i = 0; i < board->pgn.size(); ++i) {
            temp = "";
            if (i % 2 == 0) {
                sprintf(move_num_char, "%d. ", (i / 2 + 1));
                temp.append(move_num_char);
            }
            temp.append(board->pgn[i]);
            temp.append(" ");
            if ((width + temp.size()) > MAX_FILE_WIDTH) {
                last_game_pgn << "\n";
                width = 0;
            }
            last_game_pgn << temp;
            width += temp.size();
        }
        if (result.compare("*") != 0) {
            last_game_pgn << result;
        }

        last_game_pgn << "\n";
        last_game_pgn.close();
        return true;
    }
    return false;
}

void print_history_debug(Board* board) {
    for (unsigned i = 0; i < board->history.size(); ++i) {
        cout << setw(3) << i << ". ";
        cout << move_to_string_basic(board->history[i].m);
        cout << setw(13) << board->history[i].hash;
        cout << endl;
    }
}
