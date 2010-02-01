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

#ifndef BOARD_H_
#define BOARD_H_

#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <iomanip>
#include "../common/define.h"
#include "../common/utils.h"

using namespace std;

/*
 * Class to represent a chess board.
 *
 * The basic board values are set to public since we allow a fast board access
 * without function calling.
 */
class Board {
public:
    //representation of the 0x88 board
    byte board[BOARD_SIZE];

    //side to move on the board
    int to_move;

    //square on the board where the en passant is available.. NO_SQUARE otherwise
    int en_passant;

    //white's ability to castle
    int white_castle;

    //black's ability to castle
    int black_castle;

    //Keeps track of half-moves (plys) without a capture for the 50-moves rule
    int fifty_moves;

    //The number of full moves
    int full_moves;

    //Time to stop thinking for an AI player
    int stop_time;

    //Flag to stop the search on a time exit
    bool time_exit;

    //Number of checked nodes in a current search
    int checked_nodes;

    //squares where the kings are placed
    int white_king;
    int black_king;

    //captured pieces
    vector<int> white_captures;
    vector<int> black_captures;

    //history of moves
    vector<history_item> history;
    vector<string> pgn;

    // current search path
    move pv[MAX_PLY][MAX_PLY];
    int pv_length[MAX_PLY];
    bool follow_pv;
    int ply;

    Board(bool rotated = false);

    Board(const string& fen, bool rotated = false);
    Board(const Board& b);
    virtual ~Board();

    void parse_fen(string& fen);
    string preparse_fen(string& fen);
    string get_fen();

    void fake_move(move m);
    move unfake_move();

    void play_move(move m);
    //returns true if it was possible..
    bool undo_move();
    void add_pgn(string algebraic);

    int get_hash();

    void set_status(int status);
    int get_status();
    void set_inversed(int inversed);
    friend ostream & operator<<(ostream& os, Board& board);

#ifdef USE_HASH_TABLE
    htype hash_probe(int depth, int* alpha, int beta);
    void hash_store(int depth, htype type, int score, move best);
    htentry* hash_entry(int key);
#endif


private:
    //initialize values
    void inititalize();
    move parse_input(string& input);

    void initialize_hash();
    int generate_hash();
    void update_hash(move m);

    bool inversed;
    int status;

    //Current hash of this position
    int current_hash;
    int hash_pieces[PIECES][COLORS][BOARD_SIZE];
    int hash_casteling_white[CASTLE_LONG + 1]; //to use it as index
    int hash_casteling_black[CASTLE_LONG + 1]; //to use it as index
    int hash_en_passant[BOARD_SIZE];
    int hash_side;
    int hash_promotion[PIECES];

#ifdef USE_HASH_TABLE
    htentry ht[HT_SIZE];
#endif

};


#endif /* BOARD_H_ */
