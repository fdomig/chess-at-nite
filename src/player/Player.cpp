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

#include "Player.h"

Player::Player() : name("unknown"), max_thinking_time(DEFAULT_THINKING_TIME),
max_search_depth(MAX_SEARCH_DEPTH), show_best_score(false),
show_thinking(false), xboard(false) {
}

Player::~Player() {
}

string& Player::get_name() {
    return name;
}

void Player::set_board(Board* new_board) {
    board = new_board;
}

void Player::set_max_thinking_time(int seconds) {
    max_thinking_time = seconds;
}

void Player::set_max_search_depth(int depth) {
    max_search_depth = depth;
}

void Player::set_show_best_score(bool show) {
    show_best_score = show;
}

void Player::set_show_thinking(bool show) {
    show_thinking = show;
}

void Player::set_xboard(bool value) {
    xboard = value;
}
