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

#ifndef UTILS_H_
#define UTILS_H_

#include "define.h"
#include "../model/Board.h"
using namespace std;

extern byte lookup_piece(char piece);
extern void split(string& str, vector<string>& tokens, const char delimiter);
extern string piece_symbol(int piece);
extern char piece_char(int piece);
extern int get_square(const string& sq);
extern string square_to_string(int index);
extern string get_piece_unicode(int piece);
extern void print_possible_deltas(int* table);
extern void print_legal_endings(bool* table);

extern string move_to_string(move m, unsigned int length=0);
extern string empty_square_to_string(int square);
extern string move_to_string_simple(move m);
extern string move_to_string_basic(move m);
extern string move_to_string_very_basic(move m);
extern string string_to_lower(string str);
extern string string_move_algebraic(move m);
extern int get_promoted_piece(const char piece);
extern bool same_move(move m1, move m2);

extern void print_moves(vector<move>& moves);
extern void print_history(vector<history_item>& history);
extern string int_to_string(int);
extern bool is_legal_move(const vector<move>& moves, move& m);

extern int get_ms();

extern int hash_rand();
extern move string_to_move(const string& text);
extern void print_help();

extern char* display_nodes_count(float nodes);
extern char* display_score(int score);
extern char* display_time(int start, int stop);

#endif
