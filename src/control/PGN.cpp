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


#include "PGN.h"

PGN::PGN():loaded(false) {
    algebraic_moves.push_back("Nf3");
    algebraic_moves.push_back("e5");
    algebraic_moves.push_back("Qd5+");
}

PGN::~PGN(){
    
}

bool PGN::load(const string& filename) {
    ifstream file(filename.c_str());
    string line;
    if (file.is_open()) {
        while(!file.eof()) {
            getline (file, line);

            cout << line << endl;
        }
        file.close();
        return true;
    } else {
        cerr << "PGN File \"" << filename << "\" couldn't open.\n";
    }
    return false;
}

void PGN::print_info() {
    cout << "Event: " << event << endl;
    cout << "Site: " << site << endl;
    cout << "Date: " << date << endl;
    cout << "Round: " << round << endl;
    cout << "White: " << white << endl;
    cout << "Black: " << black << endl;
    cout << "Result: " << result << endl;

    cout << "Moves: ";
    for (unsigned i = 0; i < algebraic_moves.size(); ++i) {
        if (i % 2 == 0) {
            cout << endl << (i / 2 + 1) <<". ";
        }
        cout << algebraic_moves[i] << " ";
    }
    cout << endl;
}
