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

#include "utils.h"

byte lookup_piece(char piece) {
    switch (piece) {
        case 'K':
            return WHITE_KING;
        case 'Q':
            return WHITE_QUEEN;
        case 'B':
            return WHITE_BISHOP;
        case 'R':
            return WHITE_ROOK;
        case 'N':
            return WHITE_KNIGHT;
        case 'P':
            return WHITE_PAWN;
        case 'k':
            return BLACK_KING;
        case 'q':
            return BLACK_QUEEN;
        case 'b':
            return BLACK_BISHOP;
        case 'r':
            return BLACK_ROOK;
        case 'n':
            return BLACK_KNIGHT;
        case 'p':
            return BLACK_PAWN;
        default:
            return EMPTY;
    }
}

void split(const string& str, vector<string>& tokens, const char delimiter) {
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiter, 0);
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiter, lastPos);

    while (string::npos != pos || string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiter, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiter, lastPos);
    }
}

string string_to_lower(string str) {
    for (unsigned i = 0; i < strlen(str.c_str()); i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 0x20;
        }
    }
    return str;
}

/*
 * Replaces the previous piece_char
 */
string piece_symbol(int piece) {
#ifdef UNICODE
    switch (piece) {
        case WHITE_KING:
            return "\u2654";
        case WHITE_QUEEN:
            return "\u2655";
        case WHITE_ROOK:
            return "\u2656";
        case WHITE_BISHOP:
            return "\u2657";
        case WHITE_KNIGHT:
            return "\u2658";
        case WHITE_PAWN:
            return "\u2659";
        case BLACK_KING:
            return "\u265A";
        case BLACK_QUEEN:
            return "\u265B";
        case BLACK_ROOK:
            return "\u265C";
        case BLACK_BISHOP:
            return "\u265D";
        case BLACK_KNIGHT:
            return "\u265E";
        case BLACK_PAWN:
            return "\u265F";
    }
    return "x";

#else
    switch (piece) {
        case WHITE_KING:
            return "K";
        case WHITE_QUEEN:
            return "Q";
        case WHITE_ROOK:
            return "R";
        case WHITE_BISHOP:
            return "B";
        case WHITE_KNIGHT:
            return "N";
        case WHITE_PAWN:
            return "P";
        case BLACK_KING:
            return "k";
        case BLACK_QUEEN:
            return "q";
        case BLACK_ROOK:
            return "r";
        case BLACK_BISHOP:
            return "b";
        case BLACK_KNIGHT:
            return "n";
        case BLACK_PAWN:
            return "p";
    }
    return "x";
#endif
}

/*
 * Be careful! This function is only for efn generation
 * For console print-outs use piece_symbol(int piece) instead
 */
char piece_char(int piece) {
    //for white and black pieces (k and K)
    int mod = piece > 0 ? 0 : ('a' - 'A');
    piece = abs(piece);

    switch (piece) {
        case KING:
            return 'K' + mod;
        case QUEEN:
            return 'Q' + mod;
        case KNIGHT:
            return 'N' + mod;
        case ROOK:
            return 'R' + mod;
        case BISHOP:
            return 'B' + mod;
        case PAWN:
            return 'P' + mod;
    }
    return '.';
}

string square_names[] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "##", "##", "##", "##", "##", "##", "##", "##",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "##", "##", "##", "##", "##", "##", "##", "##",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "##", "##", "##", "##", "##", "##", "##", "##",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "##", "##", "##", "##", "##", "##", "##", "##",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "##", "##", "##", "##", "##", "##", "##", "##",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "##", "##", "##", "##", "##", "##", "##", "##",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "##", "##", "##", "##", "##", "##", "##", "##",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "##", "##", "##", "##", "##", "##", "##", "##",
};

string square_to_string(int index) {
    if (index == NO_SQUARE) {
        return "-";
    }
    return square_names[index];
}

/*
 * For debug purposes in Move Generator while checking for check
 */
void print_possible_deltas(int* table) {
    for (int rank = SIZE; rank >= 0; rank--) {
        cout << rank << " ";
        for (int file = 0; file < SIZE; file++) {
            if (rank != 0)
                cout << setw(3) << table[(rank - (1 << 4)) | file];
            else
                cout << setw(3) << (char) (file + 'a');
        }
        cout << endl;
    }
}

/*
 * For debug purposes in Move Generator while checking for check
 */
void print_legal_endings(bool* table) {
    for (int rank = SIZE; rank >= 0; rank--) {
        cout << rank << " ";
        for (int file = 0; file < SIZE; file++) {
            if (rank != 0)
                cout << setw(2) << (table[(rank - (1 << 4)) | file] ? 'x' : '.');
            else
                cout << setw(2) << (char) (file + 'a');
        }
        cout << endl;
    }
}

string move_to_string_simple(const move& m) {
    string result = "";
    result.append(piece_symbol(m.moved_piece));
    result.append(": ");
    result.append(square_to_string(m.pos_old));
    result.append("..");
    result.append(square_to_string(m.pos_new));
    if (m.content != EMPTY) {
        result.append(" {");
        result.append(piece_symbol(m.content));
        result.append("}");
    }
    return result;
}

/**
 * Do not add anything special.. like O-O and O-O-O. This should only return
 * departure/destination squares.
 * 
 */
string move_to_string_basic(const move& m) {
    string result;
    result = square_to_string(m.pos_old) + square_to_string(m.pos_new);
    if (m.special == MOVE_PROMOTION) {
        result += piece_symbol(-abs(m.promoted));
    }
    return result;
}

/*
 * Always returns string of 4 characters with pos_old and pos_new
 */
string move_to_string_very_basic(const move& m) {
    string result("");
    result.append(square_to_string(m.pos_old));
    result.append(square_to_string(m.pos_new));
    return result;
}

/*
 * String representation of the move..
 * define the length of that string if you want to..
 */
string move_to_string(const move& m, unsigned int length) {
    int unicodes = 0;

#ifdef UNICODE
    unicodes = 2;
    if (m.content != EMPTY) {
        unicodes += 2;
    }
#endif
    string result = move_to_string_simple(m);

    switch (m.special) {
        case MOVE_ORDINARY:
            break;
        case MOVE_PROMOTION:
#ifdef UNICODE
            unicodes += 2;
#endif
            result.append(" [" + piece_symbol(m.promoted) + "]");
            break;
        case MOVE_CASTLE_SHORT:
            result = piece_symbol(m.moved_piece) + ": O-O";
            break;
        case MOVE_CASTLE_LONG:
            result = piece_symbol(m.moved_piece) + ": O-O-O";
            break;
        case MOVE_EN_PASSANT:
            result.append("{En}");
            break;
    }

    int spaces = length - result.length() + unicodes;
    if (spaces > 0) {
        for (int i = 0; i < spaces; i++) {
            result.append(" ");
        }
    }
    return result;
}

string empty_square_to_string(int square) {
    if ((RANK(square) + FILE(square)) & 1) {
        return ".";
    } else {
        return ",";
    }
}

/**
 *
 * Translates the square from a string format to index of our board
 *  'E4' > 52
 */
int get_square(const string& sq) {
    if (toupper(sq[0]) >= 'A' && toupper(sq[0]) <= 'H' && toupper(sq[1]) >= '1' && toupper(sq[1]) <= '8') {
        return (toupper(sq[0]) - 'A') * NEXT_FILE + (toupper(sq[1]) - '1') * NEXT_RANK;
    }
    return NO_SQUARE;
}

int get_promoted_piece(const char piece) {
    switch (toupper(piece)) {
        case 'Q':
            return QUEEN;
        case 'N':
            return KNIGHT;
        case 'B':
            return BISHOP;
        case 'R':
            return ROOK;
    }
    return EMPTY;
}

bool operator ==(const move& m1, const move& m2) {
    return (m1.pos_new == m2.pos_new) && (m1.pos_old == m2.pos_old);
}

bool is_legal_move(const vector<move>& moves, move& m) {
    for (vector<move>::const_iterator it = moves.begin(); it != moves.end(); ++it) {
        if (*it == m) {
            if (m.special == MOVE_PROMOTION) {
                //if it's the promotion move.. then the moved_piece is the pawn (-1 or 1)
                //and the promoted has usually a positive value.. in order to make legal 
                // we have to change the sign!
                m.promoted = m.moved_piece * abs(m.promoted);
                return true;
            } else if (it->special == MOVE_PROMOTION && m.special != MOVE_PROMOTION) {
                return false;
            } else {
                m = *it;
                return true;
            }
        }
    }
    return false;
}

move string_to_move(const string& text) {
    move m;
    m.special = MOVE_ERROR;

    int pos_old;
    int pos_new;
    int promoted;

    //commands
    if (text.length() == 2 && text[0] == '/') {
        switch (tolower(text[1])) {
            case 'm':
                m.special = MOVE_SHOW_MOVES;
                break;
            case 'a':
                m.special = MOVE_SHOW_ALGEBRAIC;
                break;
            case 'h':
                m.special = MOVE_SHOW_HISOTRY;
                break;
            case 'b':
                m.special = MOVE_SHOW_BOARD;
                break;
            case 'u':
                m.special = MOVE_UNDO;
                break;
            case 'r':
                m.special = MOVE_RESIGN;
                break;
            case 'd':
                m.special = MOVE_DRAW;
                break;
            case '?':
                m.special = MOVE_SHOW_HELP;
                break;
        }
    } else if (text.length() >= 4 && text.length() <= 5) {
        pos_old = get_square(text.substr(0, 2));
        if (pos_old > NO_SQUARE) {
            pos_new = get_square(text.substr(2, 2));
            if (pos_new > NO_SQUARE) {
                m.pos_old = pos_old;
                m.pos_new = pos_new;
                m.special = MOVE_ORDINARY;

                if (text.length() == 5) {
                    promoted = get_promoted_piece(text[4]);
                    if (promoted != EMPTY) {
                        m.promoted = promoted;
                        m.special = MOVE_PROMOTION;
                    } else {
                        m.special = MOVE_ERROR;
                    }
                }
            }
        }
    }
    return m;
}

void print_moves(const vector<move>& moves) {
    if (moves.size() == 0) {
        cout << "No available moves... :(" << endl;
        return;
    }
    cout << "---------" << setw(2) << moves.size() << " available moves--------" << endl;
    unsigned int i;
    for (i = 0; i < moves.size(); i++) {
        cout << move_to_string(moves[i], 20);
        if ((i % 2) != 0) {
            cout << endl;
        }
    }
    if ((i % 2) != 0) {
        cout << endl;
    }
    cout << "-----------------------------------" << endl;
}

void print_history(const vector<string>& history) {
    if (history.size() == 0) {
        cout << "Nothing in the history..." << endl;
        return;
    }
    cout << "--------------History--------------" << endl;
    unsigned int i;
    for (i = 0; i < history.size(); i++) {
        if (i % 2 == 0) {
            cout << setw(3) << i / 2 + 1 << ".";
        }
        cout << " " << history[i];

        if ((i % 2) != 0) {
            cout << endl;
        }
    }
    if ((i % 2) != 0) {
        cout << endl;
    }
    cout << "-----------------------------------" << endl;
}

string int_to_string(int i) {
    stringstream ss;
    ss << i;
    return ss.str();
}

int hash_rand() {
    int i;
    int r = 0;

    for (i = 0; i < 32; ++i)
        r ^= rand() << i;
    return r;
}

int get_ms() {
    struct timeb timebuffer;
    ftime(&timebuffer);
    return (timebuffer.time * 1000) +timebuffer.millitm;
}

void print_help() {
    cout << "------------------ HELP -------------------" << endl;
    cout << "  You can enter a move by using algebraic" << endl;
    cout << "chess notation. Case matters, but you can " << endl;
    cout << "avoid special symbols (x, +, #, =, etc)." << endl;
    cout << "If you have no idea what is that, use the" << endl;
    cout << "following rules to enter your move." << endl;
    cout << "" << endl;
    cout << "Normal moves: " << endl;
    cout << " Write from/to squares (e2e4)" << endl;
    cout << "Castling moves: " << endl;
    cout << " The move of the king (e1g1)" << endl;
    cout << "Promotion moves: " << endl;
    cout << " Normal move and the promoted piece (e7e8Q)" << endl;
    cout << "   [Queen, kNight, Rook, Bishop]" << endl;
    cout << "En passant moves: " << endl;
    cout << " Normal move of a pawn (e5d6)" << endl;
    cout << "  " << endl;
    cout << "Extra commands during the game: " << endl;
    cout << " /a - algebraic notation of moves" << endl;
    cout << " /m - all the available moves" << endl;
    cout << " /h - history of moves" << endl;
    cout << " /r - resign from the game" << endl;
    cout << " /u - undo your move" << endl;
    cout << " /b - current board state" << endl;
    cout << " /? - help" << endl;
    cout << "-------------------------------------------" << endl;
}

char* display_nodes_count(float nodes) {
    static char out[10];
    if (nodes < 1000)
        sprintf(out, "%.0f", nodes);
    else if (nodes < 1000000)
        sprintf(out, "%.1fK", nodes / 1000);
    else
        sprintf(out, "%.1fM", nodes / 1000000);
    return (out);
}

char* display_score(int score) {
    static char out[10];
    if (score >= MATE) {
        sprintf(out, "+Mate");
    } else if (score <= -MATE) {
        sprintf(out, "-Mate");
    } else {
        sprintf(out, "%.2f", ((float) score) / 100.0);
    }
    return (out);
}

char* display_time(int start, int stop) {
    static char out[10];
    double end = (double) (stop - start) / 1000;
    sprintf(out, "%.2f", end);
    return (out);
}
