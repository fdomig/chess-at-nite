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
#include "Player.h"

Player::Player() {
}

Player::~Player() {
}

void Player::set_board(Board* new_board) {
    board = new_board;
}
