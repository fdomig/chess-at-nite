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

#include "MoveGenerator.h"

#define KING_MOVES    8
#define QUEEN_MOVES   8
#define ROOK_MOVES    4
#define BISHOP_MOVES  4
#define KNIGHT_MOVES  8

int DELTA_KNIGHT[] = { MV_U * 2 + MV_L, MV_U * 2 + MV_R, MV_R * 2 + MV_U, MV_R * 2 + MV_D, MV_D * 2 + MV_R, MV_D * 2 + MV_L, MV_L * 2 + MV_U, MV_L * 2 + MV_D, 0 };
int DELTA_ROOK[] = { MV_U, MV_D, MV_L, MV_R, 0 };
int DELTA_BISHOP[] = { MV_UL, MV_UR, MV_DL, MV_DR, 0 };
int DELTA_ALL[] = { MV_U, MV_D, MV_L, MV_R, MV_UL, MV_UR, MV_DL, MV_DR, 0 };
int* DELTA_QUEEN = DELTA_ALL;
int* DELTA_KING = DELTA_ALL;

//stupid pawns
int DELTA_WHITE_PAWN[] = { MV_U, MV_UL, MV_UR, 0 }; // first move is moving forward! don't change
int DELTA_BLACK_PAWN[] = { MV_D, MV_DR, MV_DL, 0 }; // first move is moving forward! don't change

#define WHITE_PAWN_START_RANK  1 //Rank: 2
#define BLACK_PAWN_START_RANK  6 //Rank: 7
MoveGenerator::MoveGenerator(Board* new_board) :
    board(new_board) {
    reset();
}

MoveGenerator::~MoveGenerator() {

}

void MoveGenerator::reset() {
    all_moves.clear();
    normal_moves.clear();
    capture_moves.clear();
    captures_of_last_moved_piece.clear();
    promotion_moves.clear();
    castling_moves.clear();

    king_under_check = false;
    king_checked_by_knight = NO_SQUARE;

    king_square = board->to_move == WHITE ? board->white_king : board->black_king;
    last_move_square = NO_SQUARE;
    if (!board->history.empty()) {
        last_move_square = board->history.back().m.pos_new;
    }

    for (int square = 0; square < BOARD_SIZE; ++square) {
        legal_delta[square] = 0;
        legal_ending[square] = false;
    }
}

void MoveGenerator::print_debug_info() {
    cout << "The table of legal deltas:" << endl;
    print_possible_deltas(legal_delta);
    cout << "The table of possible endings:" << endl;
    print_legal_endings(legal_ending);
}

void MoveGenerator::add_move(move possible_move) {
    if (last_move_square == possible_move.pos_new) {
        captures_of_last_moved_piece.push_back(possible_move);
    } else if (possible_move.content != EMPTY) {
        //capture moves here..
        capture_moves.insert(possible_move);
    } else if (possible_move.special == MOVE_CASTLE_SHORT || possible_move.special == MOVE_CASTLE_LONG) {
        castling_moves.push_back(possible_move);
    } else if (possible_move.special == MOVE_PROMOTION) {
        promotion_moves.push_back(possible_move);
    } else {
        normal_moves.push_back(possible_move);
    }
}

/*
 *
 * Capture moves should be sorted with the highest captured piece first
 *
 * All moves:
 *      add in the beginning all the moves that attacking the last moved piece!
 */
void MoveGenerator::sort_moves() {
	all_moves.reserve(captures_of_last_moved_piece.size() + capture_moves.size() + 
                      promotion_moves.size() + castling_moves.size() + normal_moves.size());
    //insert the captures of the last moved piece in the beginning of the capture list...
    all_moves.insert(all_moves.end(), captures_of_last_moved_piece.rbegin(), captures_of_last_moved_piece.rend());
    all_moves.insert(all_moves.end(), capture_moves.begin(), capture_moves.end());
    all_moves.insert(all_moves.end(), promotion_moves.begin(), promotion_moves.end());
    all_moves.insert(all_moves.end(), castling_moves.begin(), castling_moves.end());
    all_moves.insert(all_moves.end(), normal_moves.begin(), normal_moves.end());
}

//before returning .. sort the moves...!!
void MoveGenerator::generate_all_moves() {
    reset();
    int num_of_threats = check_for_check();

    // and we should make maybe the num_of_threats global
    generate_moves_king(king_square);

    /*
     * if we have more than one threat.. then the only legal moves that we can have is
     * to able to move our king in an unchecked square..!! if there is none..
     * then it's a checkmate!! gg
     */
    if (num_of_threats > 1) {
        sort_moves();
        return;
    }

    /*
     * If king was checked by a Knight.. then either we have to move our king, or eliminate
     * the opponent's Knight...!! there no other options...!!! that's a good thing :)
     */
    if (num_of_threats == 1 && king_checked_by_knight != NO_SQUARE) {
        generate_capture_moves_for_opponent_knight(king_checked_by_knight);
        sort_moves();
        return;
    }

    if (num_of_threats == 0) {
        generate_castling_moves();
    }

    int square;
    int piece;
    for (int rank = 0; rank < SIZE; rank++) {
        for (int file = 0; file < SIZE; file++) {
            square = (rank << 4) | file;
            //if it's positive that means that the current piece belongs to the current player
            piece = board->board[square] * board->to_move;
            if (piece > 0) {
                switch (piece) {
                case PAWN:
                    generate_moves_pawn(square);
                    break;
                case ROOK:
                    generate_moves_rook(square);
                    break;
                case QUEEN:
                    generate_moves_queen(square);
                    break;
                case BISHOP:
                    generate_moves_bishop(square);
                    break;
                case KNIGHT:
                    generate_moves_knight(square);
                    break;
                }
            }
        }
    }
    sort_moves();
}

/*
 * Generating all the possible moves for a single square
 *
 * Params:
 *      one_step_only: piece can only make one step (knight & king)
 */
void MoveGenerator::generate_moves(int square, int* delta, bool one_step_only) {
    int new_square;
    move current_move;
    for (; *delta; delta++) {
        for (new_square = square + *delta; !(new_square & 0x88); new_square += *delta) {

            current_move = generate_move(square, new_square);

            //if we are facing our own color then it's not possible to move there
            //and we have to change direction..
            if (current_move.moved_piece == EMPTY) {
                break;
            }

            if (legal_delta[square] == 0 || (legal_delta[square] == *delta || legal_delta[square] == OPPOSITE(*delta))) {
                if (!king_under_check || legal_ending[new_square]) {
                    add_move(current_move);
                }
            } else {
                break;
            }

            //if during the last move we had a capture.. then there is no point of continuing on that direction
            if (one_step_only || current_move.content != EMPTY) {
                break;
            }
        }
    }
}

move MoveGenerator::generate_move(int old_square, int new_square) {
    move possible_move;

    possible_move.special = MOVE_ORDINARY;
    possible_move.promoted = EMPTY;
    possible_move.content = board->board[new_square];

    //if on the new square there is a piece of the same color
    if (board->board[new_square] * board->board[old_square] > 0) {
        possible_move.moved_piece = EMPTY;
        return possible_move;
    }

    possible_move.moved_piece = board->board[old_square];
    possible_move.pos_old = old_square;
    possible_move.pos_new = new_square;

    switch (possible_move.moved_piece) {
    case WHITE_KING:
    case WHITE_ROOK:
        possible_move.promoted = board->white_castle;
        break;
    case BLACK_KING:
    case BLACK_ROOK:
        possible_move.promoted = board->black_castle;
        break;
    }

    return possible_move;
}

void MoveGenerator::generate_move_pawn(int old_square, int new_square, bool starting_pos) {
    move possible_move;
    possible_move.moved_piece = board->board[old_square];
    possible_move.content = board->board[new_square];
    possible_move.pos_old = old_square;
    possible_move.pos_new = new_square;
    possible_move.special = MOVE_ORDINARY;
    possible_move.promoted = EMPTY;

    if (!king_under_check || legal_ending[new_square]) {
        //attacks or en passant
        if (FILE(old_square) != FILE(new_square)) {
            //!! en passanto
            if (board->en_passant == new_square) {
                possible_move.special = MOVE_EN_PASSANT;
                // pawn of the opposite color!! since the flag is on the pawn is there!
                possible_move.content = -possible_move.moved_piece;
            } else if (board->board[old_square] * board->board[new_square] >= 0) {
                return;
            }
        } else if (board->board[possible_move.pos_new] != EMPTY) {
            //moving forward is possible only if the square is empty
            return;
        }
        /*
         * if you reach at this point the possible move is valid
         * but we also have to check for possible promotions.. this is only can
         * happen if a pawn will reach an edge (rank 1 or 8)
         */
        //promotions
        if (RANK(new_square) % 7 == 0) {
            possible_move.promoted = possible_move.moved_piece * QUEEN;
            possible_move.special = MOVE_PROMOTION;
            add_move(possible_move);

            possible_move.promoted = possible_move.moved_piece * KNIGHT;
            possible_move.special = MOVE_PROMOTION;
            add_move(possible_move);
        } else {
            add_move(possible_move);
        }
    }

    //to move the pawn two squares forward legally!
    if (starting_pos && FILE(old_square) == FILE(new_square) && board->board[new_square] == EMPTY) {
        //moving one more square forward no matter what color is the pawn
        possible_move.pos_new += new_square - old_square;
        if (board->board[possible_move.pos_new] == EMPTY && (!king_under_check || legal_ending[possible_move.pos_new])) {
            add_move(possible_move);
        }
    }
}

void MoveGenerator::generate_moves_pawn(int square) {
    int* delta = NULL;
    bool starting_pos = false;

    switch (board->board[square]) {
    case WHITE_PAWN:
        delta = DELTA_WHITE_PAWN;
        starting_pos = RANK(square) == WHITE_PAWN_START_RANK;
        break;
    case BLACK_PAWN:
        delta = DELTA_BLACK_PAWN;
        starting_pos = RANK(square) == BLACK_PAWN_START_RANK;
        break;
    }

    int new_square;
    for (; *delta; delta++) {
        new_square = square + *delta;
        if (!(new_square & 0x88)) {
            //first checking if it's has the direction to go.. and then if't a legal ending
            if (legal_delta[square] == 0 || (legal_delta[square] == *delta || legal_delta[square] == OPPOSITE(*delta))) {
                generate_move_pawn(square, new_square, starting_pos);
            }
        }
    }
}

void MoveGenerator::generate_moves_bishop(int square) {
    generate_moves(square, DELTA_BISHOP, false);
}

void MoveGenerator::generate_moves_knight(int square) {
    generate_moves(square, DELTA_KNIGHT, true);
}

void MoveGenerator::generate_moves_rook(int square) {
    generate_moves(square, DELTA_ROOK, false);
}

void MoveGenerator::generate_moves_queen(int square) {
    generate_moves(square, DELTA_QUEEN, false);
}

void MoveGenerator::generate_moves_king(int square) {
    int new_square;
    move current_move;
    for (int*delta = DELTA_KING; *delta; delta++) {
        new_square = square + *delta;
        if (!(new_square & 0x88)) {
            current_move = generate_move(square, new_square);
            if (current_move.moved_piece == EMPTY) {
                continue;
            }

            if (!check_for_threat(new_square, board->to_move, true)) {
                add_move(current_move);
            }
        }
    }
}

/*
 * Generates all possible moves that we can do.. in order to eliminate this knight!!
 */
void MoveGenerator::generate_capture_moves_for_opponent_knight(int square) {
    if (check_for_threat(square, OPPONENT(board->to_move), false)) {
        for (unsigned int i = 0; i < threats.size(); i++) {
            add_move(generate_move(threats[i], square));
        }
    }
}

/*
 * Assumes already that King is not under attack at the moment...
 * and the corresponding king and rook are on the initial positions if castling is possible..!!
 * If castling is possible and either king or rook is not there.. good luck with the results..
 */
void MoveGenerator::generate_castling_moves() {
    switch (board->to_move) {
    case WHITE:
        if ((board->white_castle & CASTLE_SHORT) == CASTLE_SHORT) {
            if (board->board[F1] == EMPTY && board->board[G1] == EMPTY && check_for_threat(F1, board->to_move, false) == 0 && check_for_threat(G1, board->to_move, false) == 0) {
                move current_move;
                current_move.content = EMPTY;
                current_move.moved_piece = WHITE_KING;
                current_move.pos_old = E1;
                current_move.pos_new = G1;
                current_move.special = MOVE_CASTLE_SHORT;
                add_move(current_move);
            }
        }

        if ((board->white_castle & CASTLE_LONG) == CASTLE_LONG) {
            if (board->board[B1] == EMPTY && board->board[C1] == EMPTY && board->board[D1] == EMPTY && !check_for_threat(C1, board->to_move, false) && !check_for_threat(D1, board->to_move, false)) {
                move current_move;
                current_move.content = EMPTY;
                current_move.moved_piece = WHITE_KING;
                current_move.pos_old = E1;
                current_move.pos_new = C1;
                current_move.special = MOVE_CASTLE_LONG;
                add_move(current_move);
            }
        }
        break;

    case BLACK:
        if ((board->black_castle & CASTLE_SHORT) == CASTLE_SHORT) {
            //check if E1, F1, G1 are not under attack
            if (board->board[F8] == EMPTY && board->board[G8] == EMPTY && check_for_threat(F8, board->to_move, false) == 0 && check_for_threat(G8, board->to_move, false) == 0) {
                move current_move;
                current_move.content = EMPTY;
                current_move.moved_piece = BLACK_KING;
                current_move.pos_old = E8;
                current_move.pos_new = G8;
                current_move.special = MOVE_CASTLE_SHORT;
                add_move(current_move);
            }
        }

        if ((board->black_castle & CASTLE_LONG) == CASTLE_LONG) {
            if (board->board[B8] == EMPTY && board->board[C8] == EMPTY && board->board[D8] == EMPTY && check_for_threat(C8, board->to_move, false) == 0 && check_for_threat(D8, board->to_move, false) == 0) {
                move current_move;
                current_move.content = EMPTY;
                current_move.moved_piece = BLACK_KING;
                current_move.pos_old = E8;
                current_move.pos_new = C8;
                current_move.special = MOVE_CASTLE_LONG;
                add_move(current_move);
            }
        }
        break;
    }
}

/*
 * returns all possible moves for current player!
 * if there are no moves.. then you might lost the game..!! gg
 */
vector<move>& MoveGenerator::get_all_moves() {
    return all_moves;
}

vector<move>& MoveGenerator::get_all_moves(move best_move) {
    best_move_in_front = all_moves;

    for (vector<move>::iterator iter = best_move_in_front.begin(); iter != best_move_in_front.end(); iter++) {
        if (*iter == best_move) {
            best_move_in_front.erase(iter);
            break;
        }
    }

    best_move_in_front.insert(best_move_in_front.begin(), best_move);
    return best_move_in_front;
}

/*
 * returns only the capture moves
 */
void MoveGenerator::get_all_capture_moves(vector<move>& moves) {
    moves.reserve(capture_moves.size() + captures_of_last_moved_piece.size());
    moves.insert(moves.end(), captures_of_last_moved_piece.rbegin(), captures_of_last_moved_piece.rend());
    moves.insert(moves.end(), capture_moves.begin(), capture_moves.end());
}

/*
 * Checks if the square is under attack.
 * skip your own king:
 *      when we are trying to find possible way outs of our own king.. should be true!
 *      for example: if our king is on the same rank with opponents queen.. it's impossible to
 *      move on that line.. even though the next square seems ok, cause the same king is
 *      protecting it...
 */
bool MoveGenerator::check_for_threat(int square, int color, bool skip_your_own_king) {
    // we only care if it's smaller/greater than zero...
    // sometimes we want to check if somebody can threat our piece or square
    // and sometimes we need to know how to eliminate a threat (like attacking a knight)
    threats.clear();
    int * delta = DELTA_ALL;
    int new_square;
    int depth;
    int contents;

    //checks if there is any threat from opponents stupid Knights
    for (int *delta = DELTA_KNIGHT; *delta; delta++) {
        new_square = square + *delta;
        if (!(new_square & 0x88) && board->board[new_square] * OPPONENT(color) == KNIGHT) {
            threats.push_back(new_square);
        }
    }

    //checking all possible directions
    for (; *delta; delta++) {
        depth = 0;
        for (new_square = square + *delta; !(new_square & 0x88); new_square += *delta) {
            depth++;
            contents = board->board[new_square] * color;
            //if there is an opponent pawn or king on the next square then check if it can attack
            if ((contents == -PAWN || contents == -KING)) {
                if (depth == 1) {
                    if (is_possible_to_attack(new_square, OPPOSITE(*delta))) {
                        threats.push_back(new_square);
                    }
                }
                break;
            }

            //as long as it empty continue with next square
            if (board->board[new_square] == EMPTY) {
                continue;
            }

            if (skip_your_own_king && contents == KING) {
                continue;
            }

            //if we found our own piece you can stop, but this piece might protect our king
            //it won't matter if we just checking an empty square in case we want to castle
            if (board->board[new_square] * color > 0) {
                break; //continue with another direction
            }

            //if you reach at this point that means that you found opponents piece
            //we have to check if it's a threat for the current square.
            if (is_possible_to_attack(new_square, *delta)) {
                threats.push_back(new_square);
            }

            //we have to break... cause there is no point continuing towards this direction
            break;
        }
    }

    return threats.size() > 0;
}

bool MoveGenerator::check_for_check_simple() {
    int depth;
    int piece_found;
    int new_square;

    //check for knight attacks first
    for (int *delta = DELTA_KNIGHT; *delta; delta++) {
        new_square = king_square + *delta;
        if (!(new_square & 0x88) && board->board[new_square] * OPPONENT(board->to_move) == KNIGHT) {
            return true;
            break;
        }
    }
    for (int *delta = DELTA_ALL; *delta; delta++) {
        depth = 0;
        for (new_square = king_square + *delta; !(new_square & 0x88); new_square += *delta) {
            depth++;

            //if it's positive it's opponents!!
            //if negative then it's ours
            //if 0 then it's empty
            piece_found = board->board[new_square] * OPPONENT(board->to_move);

            //as long as its empty continue with next square
            if (piece_found == EMPTY) {
                continue;
            }

            //if there is pawn on the next square then check if it can attack
            if ((piece_found == PAWN || piece_found == KING)) {
                if (depth == 1) {
                    if (is_possible_to_attack(new_square, OPPOSITE(*delta))) {
                        return true;
                    }
                }
                break;
            }

            //if we found our own piece you can stop for this direction
            if (piece_found < 0) {
                break;
            }

            //if you reach at this point that means that you found opponents piece
            //we have to check if it's a threat for the current square.
            if (is_possible_to_attack(new_square, *delta)) {
                return true;
            }
            break;
        }
    }
    return false;
}

int MoveGenerator::check_for_check() {
    int my_color = board->to_move;
    int new_square;
    int number_of_threats = 0;
    //how far we are from king
    int depth;
    //the piece that is in our way...
    int piece_found;
    //to mark the legal direction if a piece protecting our king
    int square_of_guarding_piece;
    king_under_check = false;

    //checks if there is any threat from opponents stupid Knights
    //ONLY ONE KNIGHT can check our king... if there are more than one.. it's an illegal state!!!
    for (int *delta = DELTA_KNIGHT; *delta; delta++) {
        new_square = king_square + *delta;
        if (!(new_square & 0x88) && board->board[new_square] * OPPONENT(my_color) == KNIGHT) {
            king_under_check = true;
            king_checked_by_knight = new_square;
            number_of_threats++;
            break;
        }
    }

    //checking all possible directions
    for (int *delta = DELTA_ALL; *delta; delta++) {
        depth = 0;
        square_of_guarding_piece = NO_SQUARE;
        for (new_square = king_square + *delta; !(new_square & 0x88); new_square += *delta) {
            depth++;

            //if it's positive it's opponents!!
            //if negative then it's ours
            //if 0 then it's empty
            piece_found = board->board[new_square] * OPPONENT(my_color);

            //as long as its empty continue with next square
            if (piece_found == EMPTY) {
                continue;
            }

            //if there is pawn on the next square then check if it can attack
            if ((piece_found == PAWN || piece_found == KING)) {
                if (depth == 1) {
                    if (is_possible_to_attack(new_square, OPPOSITE(*delta))) {
                        king_under_check = true;
                        number_of_threats++;
                        legal_ending[new_square] = true;
                    }
                }
                break;
            }

            //if we found our own piece you can stop, but this piece might protect our king
            if (piece_found < 0) {
                if (square_of_guarding_piece == NO_SQUARE) {
                    square_of_guarding_piece = new_square;
                    continue;
                } else {
                    break;
                }
            }

            //if you reach at this point that means that you found opponents piece
            //we have to check if it's a threat for the current square.
            if (is_possible_to_attack(new_square, *delta)) {
                if (square_of_guarding_piece == NO_SQUARE) {
                    number_of_threats++;
                    king_under_check = true;
                    legal_ending[new_square] = true;
                    //don't try do this at home...!! we are professionals...
                    for (int index = king_square + *delta; index != new_square; index += *delta) {
                        legal_ending[index] = true;
                    }
                } else {
                    legal_delta[square_of_guarding_piece] = *delta;
                }
                break;
            }
            break;
        }
    }
    return number_of_threats;
}

/*
 * Checks if a piece can move towards that direction..
 * Not suitable for knights...
 */
bool MoveGenerator::has_direction(int *delta, int direction) {
    for (; *delta; delta++) {
        if (*delta == direction) {
            return true;
        }
    }
    return false;
}

/*
 * Checks basically if it's possible from a square to attack to a specific direction
 * Not suitable for Knights
 */
bool MoveGenerator::is_possible_to_attack(int from_square, int direction) {
    switch (abs(board->board[from_square])) {
    case KING:
        return has_direction(DELTA_KING, direction);
        break;
    case QUEEN:
        return has_direction(DELTA_QUEEN, direction);
        break;
    case ROOK:
        return has_direction(DELTA_ROOK, direction);
        break;
    case BISHOP:
        return has_direction(DELTA_BISHOP, direction);
        break;
    case PAWN:
        switch (board->board[from_square]) {
        case WHITE_PAWN:
            //+1 because the first move of a pawn is move forward.. no attack there!
            return has_direction(DELTA_WHITE_PAWN + 1, direction);
        case BLACK_PAWN:
            return has_direction(DELTA_BLACK_PAWN + 1, direction);
        }
    }
    return false;
}

