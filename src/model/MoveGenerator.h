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

#ifndef MOVE_GENERATOR_H_
#define MOVE_GENERATOR_H_

#include <iostream>
#include <vector>
#include <set>
#include "../common/define.h"
#include "../common/utils.h"
#include "Board.h"

using namespace std;

class CompareCaptureMoves
{
public:
    bool operator () (const move& m1, const move& m2) const
    {
       //sort them with the highest captured value first
       return abs(m1.content) > abs(m2.content);
    }
};

class MoveGenerator {
public:
    MoveGenerator(Board* board);
    bool king_under_check;

    virtual ~MoveGenerator();

    typedef vector<move> MovesCont;
    typedef multiset<move, CompareCaptureMoves> CaptureMovesCont;

    void generate_all_moves();
    vector<move>& get_all_moves();
    void get_all_capture_moves(vector<move>& moves);
    vector<move>& get_all_moves(move best_move);

    //returns true if somebody can attack a specific square and stores in threats the attacked squares
    bool check_for_threat(int square, int color, bool skip_your_own_king);

    void print_debug_info();
    //if there is any threat on our king.. it will stop and return true!
    //simple version!
    bool check_for_check_simple();

private:
    Board* board;
    vector<move> all_moves;
    vector<move> normal_moves;
    CaptureMovesCont capture_moves;
    vector<move> captures_of_last_moved_piece;
    vector<move> promotion_moves;
    vector<move> castling_moves;
    vector<move> best_move_in_front;

    //when you are checking for threats here you can find the squares
    vector<int> threats;


    //square that the last moved piece ended
    int last_move_square;

    //if the king is checked by a knight.. it's a good case!
    int king_checked_by_knight;

    //current player's king position on the board
    int king_square;
    /*
     * If king is under check then you should end up in a legal square from that array
     */
    bool legal_ending[BOARD_SIZE];

    /*
     * if a piece on that square !=0 then it contains the legal direction that this piece can move
     */
    int legal_delta[BOARD_SIZE];
    bool under_check;

    void reset();
    void add_move(move possible_move);
    void generate_moves(int square, int* delta, bool one_step_only);
    move generate_move(int old_square, int new_square);

    void generate_move_pawn(int old_square, int new_square, bool starting_pos);
    void generate_moves_pawn(int square);
    void generate_moves_bishop(int square);
    void generate_moves_knight(int square);
    void generate_moves_rook(int square);
    void generate_moves_queen(int square);
    void generate_moves_king(int square);
    void generate_castling_moves();
    void generate_capture_moves_for_opponent_knight(int square);

    int check_for_check();


    bool is_possible_to_attack(int from_square, int direction);
    bool has_direction(int *delta, int direction);
    void sort_moves();
};

#endif /* MOVE_H_ */
