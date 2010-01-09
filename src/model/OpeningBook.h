/*
 * ChessAtNite
 *
 * Written by:
 *   Franziskus Domig
 *   Panayiotis Lipiridis
 *   Radoslav Petrik
 *   Thai Gia Tuong
 *
 * Chess implementation for AI course during 5th semester at IHK
 *
 */

#ifndef OPENINGBOOK_H_
#define OPENINGBOOK_H_

#include <vector>
#include <fstream>
#include <iostream>

#include "../common/define.h"
#include "Board.h"
#include "../common/utils.h"

using namespace std;

class OpeningBook {
public:
    OpeningBook();
    ~OpeningBook();

    bool get_move(vector<move>& available_moves, vector<history_item>& made_moves, move& m);
    void print();

private:
    void parse_book();
    bool set_first_and_last(string last_move_string, unsigned depth);
    bool validate_move(move move_move, vector<move>& available_moves, move* valid_move);
    ifstream book_file;
    bool book_open;
    vector<vector<string> > book;
    unsigned int first;
    unsigned int last;
    unsigned int max_line_size;
    string last_move_string;
    unsigned int last_move_index;

};

#endif /* OPENINGBOOK_H_ */
