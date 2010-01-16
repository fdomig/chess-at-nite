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

#include "OpeningBook.h"

OpeningBook::OpeningBook() {
	srand(time(NULL));
	book_file.open(OPENING_BOOK);
	max_line_size = 0;
	book_open = false;
	if (book_file) {
		//if there is any correct line then the book will be open!
		parse_book();
	} else {
		cerr << "Opening book file (book) is missing." << endl;
	}
	first = 0;
	last = book.size() - 1;
	book_file.close();
}

/* close_book() closes the book file. This is called when the program exits. */

OpeningBook::~OpeningBook() {

}

bool OpeningBook::get_move(vector<move>& available_moves,
        vector<history_item>& made_moves, move& m) {
	/*
	 * Check if the book is still open, if closed return false
	 */
	if (!book_open) {
		return false;
	}

	/*
	 * Initialize your variables
	 */
	int random = rand();
	move valid_move;
	move last_move;
	string move_string;
	move move_move;
	string last_move_string;
	//    int next_move = 0;

	/*
	 * Check if we are dealing with the first move of the game or not
	 */
	if (made_moves.size() == 0) {
		move_string = book[random % book.size()][0];
		move_move = string_to_move(move_string);
		if (validate_move(move_move, available_moves, &valid_move)) {
			m = valid_move;
			set_first_and_last(move_string, 0);
			return true;
		} else {
			// Something went wrong, book doesn't contain valid moves for this board configuration, close the book
			book_open = false;
			return false;
		}
	}

	/*
	 * Check if we aren't too far in the game already, if so close the book and return false
	 */

	if (made_moves.size() >= max_line_size) {
		book_open = false;
		return false;
	}

	//Check if the last move is in the book.
	last_move = made_moves.back().m;
	last_move_string = move_to_string_very_basic(last_move);
	if (!set_first_and_last(last_move_string, made_moves.size() - 1)) {
		book_open = false;
		return false;
	}
	unsigned int index_to_be_returned = first + random % (last - first + 1);
	if (book[index_to_be_returned].size() <= made_moves.size()) {
		book_open = false;
		return false;
	}

	move_string = book[index_to_be_returned][made_moves.size()];

	move new_move = string_to_move(move_string);
	if (validate_move(new_move, available_moves, &valid_move)) {
		m = last_move = valid_move;
		set_first_and_last(move_string, made_moves.size());
		return true;
	} else {
		// Something went wrong, book doesn't contain valid moves for this board configuration, close the book
		book_open = false;
		return false;
	}
}

bool OpeningBook::validate_move(move move_move, vector<move>& available_moves,
        move* valid_move) {
	for (unsigned int index = 0; index < available_moves.size(); index++) {
		if (same_move(move_move, available_moves[index])) {
			*valid_move = available_moves[index];
			return true;
		}
	}
	return false;
}

bool OpeningBook::set_first_and_last(string last_move_string, unsigned depth) {
	bool entry_found = false;
	if (first > last) {
		cerr
		        << "You should have never seen this message. Something must have gone terribly wrong in the book"
		        << endl;
	}
	last_move_string = string_to_lower(last_move_string);
	for (unsigned int index = first; index <= last; index++) {
		if (book[index].size() > depth) {
			if (book[index][depth].compare(last_move_string) == 0) {
				first = index;
				entry_found = true;
				break;
			}
		}
	}
	if (!entry_found) {
		book_open = false;
		return false;
	}
	for (unsigned int index = first; index <= last; index++) {
		if (book[index].size() > depth) {
			if (book[index][depth].compare(last_move_string) != 0) {
				last = index - 1;
				break;
			}
		}
	}
	return true;
}

void OpeningBook::parse_book() {
	string line = "";
	vector<string> book_line;
	bool valid;
	vector<string> turns;
	unsigned int line_size;
	int line_number = 0;
	while (!book_file.eof()) {
		getline(book_file, line);
		valid = line.size() >= 9;
		if (valid) {
			split(line, turns, ' ');
			line_size = turns.size();
			bool error_at_line = false;
			for (unsigned int index = 0; index < line_size; index++) {
				string turn = turns[index];
				valid = turn.size() == 4 && turn[0] >= 'a' && turn[0] <= 'h'
				        && turn[1] >= '1' && turn[1] <= '8' && turn[2] >= 'a'
				        && turn[2] <= 'h' && turn[3] >= '1' && turn[3] <= '8';
				if (valid) {
					book_line.push_back(turn);
				} else {
					cerr << "Wrong book entry at line: " << line_number
					        << " index: " << index << " in book: "
					        << OPENING_BOOK << endl;
					error_at_line = true;
					break;
				}
			}
			if (!error_at_line) {
				if (line_size > max_line_size) {
					max_line_size = line_size;
				}
				book.push_back(book_line);
				book_open = true;
			}
			book_line.clear();
			turns.clear();
		} else {
			//to skip empty lines..
			if (line.size() > 5) {
				cerr << "Wrong book line: " << line_number << endl;
			}
		}
		line_number++;
	}
}

void OpeningBook::print() {
	cout << "Number of entries: " << book.size() << endl;
	cout << "Longest line: " << max_line_size << endl;
	for (unsigned int line = 0; line < book.size(); line++) {
		for (unsigned int column = 0; column < book[line].size(); column++)
			cout << book[line][column] << " ";
		cout << endl;
	}
}
