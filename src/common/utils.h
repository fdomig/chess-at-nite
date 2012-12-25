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

#ifndef UTILS_H_
#define UTILS_H_

#include <string.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <sstream>
#include <sys/timeb.h>
#include <stdio.h>

#include "define.h"

using std::cout;
using std::endl;
using std::setw;
using std::stringstream;

extern byte lookup_piece(char piece);
extern void split(const std::string& str, std::vector<std::string>& tokens, const char delimiter);
extern std::string piece_symbol(int piece);
extern char piece_char(int piece);
extern int get_square(const std::string& sq);
extern std::string square_to_string(int index);
extern std::string get_piece_unicode(int piece);
extern void print_possible_deltas(int* table);
extern void print_legal_endings(bool* table);

extern std::string move_to_string(const move& m, unsigned int length=0);
extern std::string empty_square_to_string(int square);
extern std::string move_to_string_simple(const move& m);
extern std::string move_to_string_basic(const move& m);
extern std::string move_to_string_very_basic(const move& m);
extern std::string string_to_lower(std::string str);
extern int get_promoted_piece(const char piece);
bool operator == (const move& m1, const move& m2);

extern void print_moves(const std::vector<move>& moves);
extern void print_history(const std::vector<std::string>& history);
extern std::string int_to_string(int);
extern bool is_legal_move(const std::vector<move>& moves, move& m);

extern int get_ms();

extern int hash_rand();
extern move string_to_move(const std::string& text);
extern void print_help();

extern char* display_nodes_count(float nodes);
extern char* display_score(int score);
extern char* display_time(int start, int stop);

#endif
