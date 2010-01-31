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

#include "PGN.h"

PGN::PGN() {
    init();
}

PGN::~PGN() {
    delete board;
}

void PGN::init() {
    event = "?";
    site = "?";
    date = "?";
    white = "?";
    black = "?";
    result = "?";
    round = "?";
    algebraic_moves.clear();
    string fen = DEFAULT_FEN;
    board = new Board(fen);
}

Board& PGN::get_board() {
    return *board;
}

bool PGN::read_from_file(const string& file_name) {
    init();
    ifstream fh(file_name.c_str());
    if (fh.fail() || fh.bad() || fh.eof()) {
        cerr << "Unable to open PGN file: " << file_name << endl;
        return false;
    }

    while (!fh.eof()) {
        string tmp;
        fh >> tmp; //read next word (we assume all different elements are white-space seperated!).

        char end_c = 0;
        if (tmp[0] == '[')
            end_c = ']';
        else if (tmp[0] == '{')
            end_c = '}';

        if (end_c != 0) {
            // parse tags and comments
            string tagName = tmp.substr(1); //keep 1st word of field seperately (for tag name identification)
            string tagData;
            char c = 0;
            while (!fh.eof() && c != end_c) {
                fh >> c;
                if (c != end_c) {
                    if (tagName.empty() && c == ' ')
                        tagName.swap(tagData); //keep 1st word of field seperately (for tag name identification)
                    else
                        tagData += c;
                }
            }
            if (end_c != '}') {
                // check header for player name tags
                std::transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);
                if (tagName == "white")
                    white = tagData;
                else if (tagName == "black")
                    black = tagData;
                else if (tagName == "event")
                    event = tagData;
                else if (tagName == "date")
                    date = tagData;
                else if (tagName == "site")
                    site = tagData;
                else if (tagName == "round")
                    round = tagData;
                else if (tagName == "result")
                    result = tagData;

            }
        } else if (tmp.find('.') == tmp.npos) {
            // not a position number (these end in . or ...)
            if (tmp == "1/2-1/2" || tmp == "1-0" || tmp == "0-1" || tmp == "*") {
                break;
            } else if (tmp.size() > 0) {
                //if not an end-of-game semantic, then it is a move. We assume that moves
                //appear in order!
                algebraic_moves.push_back(tmp);
            }
        }
    }
    return create_board();
}

bool PGN::create_board() {
    for (unsigned i = 0; i < algebraic_moves.size(); ++i) {
        move m = algebraic_to_move(algebraic_moves[i], *board);
        if (m.special != MOVE_ERROR) {
            //just to make sure that this is the right format!
            board->add_pgn(move_to_algebraic(m, *board));
            board->play_move(m);
        } else {
            return false;
        }
    }
    return true;
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
            cout << endl << (i / 2 + 1) << ". ";
        }
        cout << algebraic_moves[i] << " ";
    }
    cout << endl;
}
