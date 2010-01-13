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

#ifndef COMPUTERPLAYER_H_
#define COMPUTERPLAYER_H_

#include "Player.h"
#include <time.h>
#include "../model/OpeningBook.h"
#include "../model/evaluate.h"
#include <memory.h>

class ComputerPlayer: public Player {
	private:
		OpeningBook opening_book;
		move search_pv();
		int alpha_beta(int depth, int alpha, int beta);
		int quiescence(int alpha, int beta);
	public:
		ComputerPlayer();
		move get_move();
};

#endif /* COMPUTERPLAYER_H_ */
