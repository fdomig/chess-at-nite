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

#ifndef HUMANPLAYER_H_
#define HUMANPLAYER_H_


#include "Player.h"
#include <string>

using namespace std;

class HumanPlayer: public Player {
	public:
		move get_move();

	protected:
		move get_move_from_user();
		move get_valid_move_from_user();
};

#endif /* HUMANPLAYER_H_ */
