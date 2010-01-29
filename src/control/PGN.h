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


#ifndef _PGN_H
#define	_PGN_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class PGN {

public:
    PGN();
    virtual ~PGN();
    bool load(const string& filename);
    void print_info();
private:
    bool loaded;
    string event;
    string site;
    string date;
    string round;
    string white;
    string black;
    string result;
    vector<string> algebraic_moves;
};

#endif	/* _PGN_H */

