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


#include "extra_utils.h"

int board_status(const Board& board) {
    Board b = Board(board);
    MoveGenerator move_generator = MoveGenerator(&b);
    move_generator.generate_all_moves();
    if (move_generator.get_all_moves().empty()) {
        if (move_generator.king_under_check) {
            return OPPONENT(b.to_move) ? STATUS_WHITE_CHECKMATE : STATUS_BLACK_CHECKMATE;
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

    vector<move> same;
    //disambiguating moves
    //same piece and same destination
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
    MoveGenerator move_generator = MoveGenerator(&b);
    move_generator.generate_all_moves();

    string algebraic = move_to_algebraic_simple(m, move_generator.get_all_moves());

    b.play_move(m);
    int status = board_status(b);
    switch (status) {
        case STATUS_CHECK:
            algebraic.append("+");
            break;
        case STATUS_WHITE_CHECKMATE:
        case STATUS_BLACK_CHECKMATE:
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
    MoveGenerator move_generator = MoveGenerator(&b);
    move_generator.generate_all_moves();
    vector<move> moves = move_generator.get_all_moves();

    for (unsigned int i = 0; i < moves.size(); i++) {
        if (strip_algebraic(algebraic).compare(
                strip_algebraic(move_to_algebraic(moves[i], board))) == 0) {
            return moves[i];
        }
    }

    return string_to_move(algebraic);
}
