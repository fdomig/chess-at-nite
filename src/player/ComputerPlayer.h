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

#ifndef COMPUTERPLAYER_H_
#define COMPUTERPLAYER_H_

#include <memory.h>
#include <time.h>
#include "Player.h"
#include "../model/OpeningBook.h"
#include "../model/evaluate.h"

class ComputerPlayer: public Player {
	private:
		OpeningBook opening_book;
		move search_pv();
		int alpha_beta(int depth, int alpha, int beta);
		int quiescence(int alpha, int beta);
		void sort_pv(vector<move>& moves);
	public:
		ComputerPlayer();
		move get_move();
};

#endif /* COMPUTERPLAYER_H_ */
