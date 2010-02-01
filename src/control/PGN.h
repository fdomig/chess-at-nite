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

#ifndef _PGN_H
#define	_PGN_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "../common/extra_utils.h"

using namespace std;

class PGN {

public:
    PGN();
    virtual ~PGN();

    bool read_from_file(const string& file_name);
    void print_info();
    Board& get_board();
private:
    string event;
    string site;
    string date;
    string round;
    string white;
    string black;
    string result;
    Board* board;
    vector<string> algebraic_moves;
    void init();
    bool create_board();
};

#endif	/* _PGN_H */

