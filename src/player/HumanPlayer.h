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

#ifndef HUMANPLAYER_H_
#define HUMANPLAYER_H_


#include <string>
#include "Player.h"

using namespace std;

class HumanPlayer : public Player {
public:
    HumanPlayer();
    move get_move();

protected:
    move get_move_from_user();
    move get_valid_move_from_user();
};

#endif /* HUMANPLAYER_H_ */
