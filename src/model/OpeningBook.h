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

#ifndef OPENINGBOOK_H_
#define OPENINGBOOK_H_

#include <vector>
#include <fstream>
#include <iostream>
#include <time.h>

#include "../common/define.h"
#include "../common/utils.h"
#include "Board.h"

using namespace std;

class OpeningBook {
public:
    OpeningBook();
    ~OpeningBook();

    bool get_move(vector<move>& available_moves, vector<history_item>& made_moves, move& m);
    void print_debug_info();
    bool is_opened();
private:
    bool book_open;
    void parse_book();
    bool set_first_and_last(string last_move_string, unsigned depth);
    bool validate_move(move move_move, vector<move>& available_moves, move* valid_move);
    ifstream book_file;
    vector<vector<string> > book;
    unsigned int first;
    unsigned int last;
    unsigned int max_line_size;
    string last_move_string;
    unsigned int last_move_index;
};

#endif /* OPENINGBOOK_H_ */
