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

#include "Board.h"

Board::Board() {
    inititalize();
}

Board::Board(const Board& b) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[i] = b.board[i];
    }
    black_castle = b.black_castle;
    white_castle = b.white_castle;
    en_passant = b.en_passant;
    fifty_moves = b.fifty_moves;
    full_moves = b.full_moves;
    for (unsigned i = 0; i < b.history.size(); i++) {
        history[i] = b.history[i];
    }
    stop_time = b.stop_time;
    to_move = b.to_move;
    checked_nodes = b.checked_nodes;
    time_exit = b.time_exit;
}

Board::Board(string& fen) {
    inititalize();
    vector<string> tokens;
    split(fen, tokens, ' ');

    string simple = preparse_fen(tokens[0]);
    parse_fen(simple);

    // the second token represents the color to move
    to_move = tokens[1].compare("w") == 0 ? WHITE : BLACK;

    // the third token represents the ability to castle
    // a "-" means no castle is available
    // "KQkq" means both sides are able to castle long and short
    // (king-side, queen-side)
    white_castle = CASTLE_NONE;
    black_castle = CASTLE_NONE;
    if (tokens[2].compare("-") != 0) {
        for (unsigned i = 0; i < tokens[2].length(); i++) {
            switch (tokens[2][i]) {
            case 'k':
                black_castle = black_castle | CASTLE_SHORT;
                break;
            case 'q':
                black_castle = black_castle | CASTLE_LONG;
                break;
            case 'K':
                white_castle = white_castle | CASTLE_SHORT;
                break;
            case 'Q':
                white_castle = white_castle | CASTLE_LONG;
                break;
            }
        }
    }

    // the forth token is for a possible en-passant move
    if (tokens[3].compare("-") != 0) {
        en_passant = get_square(tokens[3]);
    } else {
        en_passant = NO_SQUARE;
    }

    // the fifth token is for the half moves since the last capture/pawn
    // advance
    fifty_moves = atoi(tokens[4].c_str());

    // the sixth token is for the number of full moves
    full_moves = atoi(tokens[5].c_str());
}

Board::~Board() {
}

void Board::inititalize() {
    status = STATUS_NORMAL;
    for (unsigned int i = 0; i < BOARD_SIZE; i++) {
        board[i] = EMPTY;
    }
    to_move = WHITE;
    en_passant = NO_SQUARE;
    white_castle = CASTLE_LONG | CASTLE_SHORT;
    black_castle = CASTLE_LONG | CASTLE_SHORT;
    fifty_moves = 0;
    full_moves = 1;

    history.clear();
    white_captures.clear();
    black_captures.clear();

    time_exit = false;
    checked_nodes = 0;

    initialize_hash();
}

string Board::preparse_fen(string& fen) {
    string temp = "";
    for (unsigned int i = 0; i < fen.size(); i++) {
        if (fen[i] >= '1' && fen[i] <= '8') {
            for (int spaces = 0; spaces < (fen[i] - '0'); spaces++) {
                temp += ' ';
            }
        } else {
            temp += fen[i];
        }
    }
    return temp;
}

// Castling flags need to be add, possibly also en passant, half-move clock and full-move number.
void Board::parse_fen(string& fen) {
    vector<string> ranks;
    split(fen, ranks, '/');
    int square;
    for (unsigned int rank = 0; rank < ranks.size(); rank++) {
        for (unsigned int file = 0; file < ranks[rank].size(); file++) {
            square = (7 - rank) * NEXT_RANK + file * NEXT_FILE;
            board[square] = lookup_piece(ranks[rank][file]);
            switch (board[square]) {
            case WHITE_KING:
                white_king = square;
                break;
            case BLACK_KING:
                black_king = square;
                break;
            }
        }
    }
}


void Board::set_status(int s) {
    status = s;
}

int Board::get_status() {
    return status;
}

string Board::get_fen() {
    string fen = "";
    int count = 0;
    for (int rank = SIZE - 1; rank >= 0; rank--) {
        for (int file = 0; file < SIZE; file++) {
            if ((board[(rank << 4) | file]) != EMPTY) {
                if (count) {
                    fen.push_back((char) ('0' + count));
                    count = 0;
                }
                fen.push_back(piece_char(board[(rank << 4) | file]));
            } else {
                count++;
            }
        }
        if (count) {
            fen.push_back((char) ('0' + count));
            count = 0;
        }
        if (rank != 0) {
            fen.push_back('/');
        }
    }
    fen.push_back(' ');
    fen.push_back((to_move == WHITE ? 'w' : 'b'));
    fen.push_back(' ');
    if (white_castle & CASTLE_SHORT) {
        fen.push_back('K');
    }
    if (white_castle & CASTLE_LONG) {
        fen.push_back('Q');
    }
    if (black_castle & CASTLE_SHORT) {
        fen.push_back('k');
    }
    if (black_castle & CASTLE_LONG) {
        fen.push_back('q');
    }
    if (!(white_castle << 2 | black_castle)) {
        fen.push_back('-');
    }
    fen.append(" ");
    fen.append(get_square_by_index(en_passant));
    fen.append(" ");
    fen.append(to_string(fifty_moves));
    fen.append(" ");
    fen.append(to_string(full_moves));
    return fen;
}

/*
 * Castling issues:
 *  When King is moving... no matter what the castling flags are going down..
 *      but saving the previous value into promotion section of the move
 *  When a Rook is moving we have to check from which square is trying to move
 *      and if it's an initial square we are removing the correct flag from corresponding
 *      castling and saving that into promotion section in order to be able to go back!
 *
 */
void Board::fake_move(move m) {
    history_item last_item;
    last_item.m = m;
    last_item.white_castle = white_castle;
    last_item.black_castle = black_castle;
    last_item.en_passant = en_passant;
    last_item.fifty_moves = fifty_moves;
    en_passant = NO_SQUARE;

    fifty_moves++;
    if (to_move == BLACK) {
        full_moves++;
    }

    switch (m.special) {
    case MOVE_ORDINARY:
        board[m.pos_old] = EMPTY;
        board[m.pos_new] = m.moved_piece;

        if (m.content != EMPTY) {
            fifty_moves = 0;
        }
        switch (m.moved_piece) {
        case WHITE_KING:
            white_king = m.pos_new;
            if (m.pos_old == WHITE_KING_INIT_SQUARE) {
                white_castle = CASTLE_NONE;
            }
            break;
        case BLACK_KING:
            black_king = m.pos_new;
            if (m.pos_old == BLACK_KING_INIT_SQUARE) {
                black_castle = CASTLE_NONE;
            }
            break;
        case WHITE_ROOK:
            switch (m.pos_old) {
            case WHITE_ROOK_SHORT_SQUARE:
                white_castle &= ~CASTLE_SHORT;
                break;
            case WHITE_ROOK_LONG_SQUARE:
                white_castle &= ~CASTLE_LONG;
                break;
            }
            break;
        case BLACK_ROOK:
            switch (m.pos_old) {
            case BLACK_ROOK_SHORT_SQUARE:
                black_castle &= ~CASTLE_SHORT;
                break;
            case BLACK_ROOK_LONG_SQUARE:
                black_castle &= ~CASTLE_LONG;
                break;
            }
            break;
        case BLACK_PAWN:
        case WHITE_PAWN:
            //if the pawn is moving two ranks.. then the square that it's between is the en passant square
            if (abs((m.pos_new - m.pos_old) >> 1) == NEXT_RANK) {
                en_passant = ((m.pos_new - m.pos_old) >> 1) + m.pos_old;
            }
            fifty_moves = 0;
            break;
        }
        break;
    case MOVE_CASTLE_SHORT:
        //king
        board[m.pos_new] = m.moved_piece;
        board[m.pos_old] = EMPTY;
        //rook
        board[m.pos_old + NEXT_FILE] = board[m.pos_old + CASTLING_SHORT_DIST_ROOK * NEXT_FILE];
        board[m.pos_old + CASTLING_SHORT_DIST_ROOK * NEXT_FILE] = EMPTY;
        //white moves
        if (m.moved_piece == WHITE_KING) {
            white_castle = CASTLE_NONE;
            white_king = m.pos_new;
        } else {
            black_castle = CASTLE_NONE;
            black_king = m.pos_new;
        }
        break;
    case MOVE_CASTLE_LONG:
        //king
        board[m.pos_new] = m.moved_piece;
        board[m.pos_old] = EMPTY;
        //rook
        board[m.pos_old - NEXT_FILE] = board[m.pos_old - CASTLING_LONG_DIST_ROOK * NEXT_FILE];
        board[m.pos_old - CASTLING_LONG_DIST_ROOK * NEXT_FILE] = EMPTY;

        if (m.moved_piece == WHITE_KING) {
            white_castle = CASTLE_NONE;
            white_king = m.pos_new;
        } else {
            black_castle = CASTLE_NONE;
            black_king = m.pos_new;
        }
        break;
    case MOVE_PROMOTION:
        board[m.pos_old] = EMPTY;
        board[m.pos_new] = m.promoted;
        fifty_moves = 0;
        break;
    case MOVE_EN_PASSANT:
        board[m.pos_old] = EMPTY;
        board[m.pos_new] = m.moved_piece;
        board[m.pos_new - m.moved_piece * NEXT_RANK] = EMPTY;
        fifty_moves = 0;
        break;
    }
    to_move = to_move == WHITE ? BLACK : WHITE;

    ply++;
    update_hash(m);
    last_item.hash = current_hash;
    history.push_back(last_item);
}

/*
 * Be careful... use it at your own risk.. if there is no move and you are trying to undo...
 * then something unexpected will happen..!!
 */
move Board::unfake_move() {
    //shouldn't happen but just to be aware..
    if (history.empty()) {
        cerr << "Something went terribly wrong.. you are trying to unfake_move and there is nothing in the history!!" << endl;
        move m;
        m.moved_piece = EMPTY;
        return m;
    }

    history_item last_item = history.back();
    history.pop_back();

    //update the previous flags
    white_castle = last_item.white_castle;
    black_castle = last_item.black_castle;
    en_passant = last_item.en_passant;
    fifty_moves = last_item.fifty_moves;
    update_hash(last_item.m);

    move m = last_item.m;
    switch (m.special) {
    case MOVE_ORDINARY:
    case MOVE_PROMOTION:
        board[m.pos_new] = m.content;
        board[m.pos_old] = m.moved_piece;
        switch (m.moved_piece) {
        case WHITE_KING:
            white_king = m.pos_old;
            break;
        case BLACK_KING:
            black_king = m.pos_old;
            break;
        }
        break;
    case MOVE_CASTLE_SHORT:
        board[m.pos_old] = m.moved_piece;
        board[m.pos_old + CASTLING_SHORT_DIST_ROOK * NEXT_FILE] = board[m.pos_old + NEXT_FILE];
        board[m.pos_new] = EMPTY;
        board[m.pos_old + NEXT_FILE] = EMPTY;
        switch (m.moved_piece) {
        case WHITE_KING:
            //the previous white_castle flags are stored in promoted...
            white_king = m.pos_old;
            break;
        case BLACK_KING:
            //the previous black_castle flags are stored in promoted...
            black_king = m.pos_old;
            break;
        }
        break;
    case MOVE_CASTLE_LONG:
        board[m.pos_old] = m.moved_piece;
        board[m.pos_old - CASTLING_LONG_DIST_ROOK * NEXT_FILE] = board[m.pos_old - NEXT_FILE];
        board[m.pos_new] = EMPTY;
        board[m.pos_old - NEXT_FILE] = EMPTY;
        switch (m.moved_piece) {
        case WHITE_KING:
            //the previous white_castle flags are stored in promoted...
            white_king = m.pos_old;
            break;
        case BLACK_KING:
            //the previous black_castle flags are stored in promoted...
            black_king = m.pos_old;
            break;
        }
        break;
    case MOVE_EN_PASSANT:
        board[m.pos_old] = m.moved_piece;
        board[m.pos_new] = EMPTY;
        board[m.pos_new - m.moved_piece * NEXT_RANK] = m.content;
        break;
    }
    to_move = to_move == WHITE ? BLACK : WHITE;

    if (to_move == BLACK) {
        full_moves--;
    }
    ply--;
    return last_item.m;
}

/*
 * This is an expensive function! Should be called only when the actual move is
 * going to be played for the game.. if you want to use something for finding
 * a best move.. use the fake_move!!!
 */
void Board::play_move(move m) {
    fake_move(m);
    if (m.content > EMPTY) {
        black_captures.push_back(m.content);
    } else if (m.content < EMPTY) {
        white_captures.push_back(m.content);
    }
    number_of_moves++;
}

/*
 * Because we do more than just undo if it's an actual undo by a human player
 */
void Board::undo_move() {
    if (!history.empty()) {
        move last_move = unfake_move();
        if (last_move.content > EMPTY) {
            if (!black_captures.empty()) {
                black_captures.pop_back();
            }
        } else if (last_move.content < EMPTY) {
            if (!white_captures.empty()) {
                white_captures.pop_back();
            }
        }
        number_of_moves--;
    }
}

void Board::initialize_hash() {
    // pieces
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 120; k++) {
                hash_pieces[i][j][k] = hash_rand();
            }
        }
    }
    // en passant
    for (int i = 0; i < 120; i++) {
        hash_en_passant[i] = hash_rand();
    }
    // castling options
    for (int i = 0; i < 4; i++) {
        hash_casteling_black[i] = hash_rand();
        hash_casteling_white[i] = hash_rand();
    }

    // promotion pieces
    for (int i = 0; i < 6; i++) {
        hash_promotion[i] = hash_rand();
    }

    // side
    hash_side = hash_rand();

    // generate current hash for start positions
    current_hash = generate_hash();
}

int Board::get_hash() {
    return current_hash;
}

void Board::update_hash(move m) {
    int key = current_hash;
    int color = m.moved_piece > 0 ? 0 : 1;
    int piece = abs(m.moved_piece) - 1;
    key ^= hash_pieces[piece][color][m.pos_old];
    key ^= hash_pieces[piece][color][m.pos_new];

    if (m.content != 0) {
        if (m.special == MOVE_EN_PASSANT) {
            key ^= hash_pieces[PAWN][color][m.pos_new - m.moved_piece * NEXT_RANK];
        } else {
            key ^= hash_pieces[abs(m.content) - 1][color][m.pos_new];
        }
    }

    switch (m.special) {
    case MOVE_CASTLE_LONG:
        if (color == WHITE) {
            key ^= hash_casteling_white[CASTLE_LONG];
        } else {
            key ^= hash_casteling_black[CASTLE_LONG];
        }
        break;
    case MOVE_CASTLE_SHORT:
        if (color == WHITE) {
            key ^= hash_casteling_white[CASTLE_SHORT];
        } else {
            key ^= hash_casteling_black[CASTLE_SHORT];
        }
        break;
    case MOVE_EN_PASSANT:
        key ^= hash_en_passant[m.pos_new - m.moved_piece * NEXT_RANK];
        break;
    case MOVE_PROMOTION:
        key ^= hash_promotion[abs(m.promoted) - 1];
    }
    current_hash = key;
}

int Board::generate_hash() {
    int key = 0;
    for (int i = 0; i < 120; i++) {
        int piece = board[i];
        if (piece > 0) {
            key ^= hash_pieces[piece - 1][0][i];
        } else if (piece < 0) {
            key ^= hash_pieces[abs(piece) - 1][1][i];
        }
    }
    key ^= hash_casteling_white[white_castle];
    key ^= hash_casteling_black[black_castle];
    if (to_move == BLACK)
        key ^= hash_side;
    return key;
}

#ifdef USE_HASH_TABLE
htype Board::hash_probe(int depth, int* alpha, int beta) {
    htentry* entry = hash_entry(current_hash);
    if (entry->key == current_hash) {
        if (entry->depth >= depth) {
            switch(entry->type) {
                case EXACT:
                    *alpha = entry->score;
                    return EXACT;
                case LOWER:
                    if (entry->score >= beta)
                        return LOWER;
                    break;
                case UPPER:
                    if (entry->score <= *alpha)
                        return UPPER;
                    break;
                case NO:
                    return NO;
            }
        }
    }
    return NO;
}

htentry* Board::hash_entry(int key) {
    return &ht[(unsigned) key % HT_SIZE];
}

void Board::hash_store(int depth, htype type, int score, move best) {
    htentry* entry = hash_entry(current_hash);
    entry->best = best;
    entry->depth = depth;
    entry->key = current_hash;
    entry->score = score;
    entry->type = type;
    entry->wtm = to_move;
}
#endif // USE_HASH_TABLE

#ifdef UNICODE
ostream& operator<<(ostream& os, Board& board) {
    int square;

    bool inverse = false;
    unsigned int index;

#ifdef TOURNAMENT
    os << endl << board.get_fen() << endl;
#endif

    int new_rank;

    os << B_RD << B_LR << B_LR << B_LR << B_LRD << B_LR;

    for (index = 0; index < SIZE; index++) {
        os << B_LR << B_LR;
    }
    os << B_LD << endl;

    for (int rank = SIZE; rank > 0; rank--) {
        new_rank = rank;
        if (inverse) {
            new_rank = SIZE - rank + 1;
        }
        os << B_UD << " " << new_rank << " " << B_UD << " ";
        for (int file = 0; file < SIZE; file++) {
            square = (new_rank - 1) * NEXT_RANK + file * NEXT_FILE;
            if (board.board[square] != EMPTY) {
                os << piece_symbol(board.board[square]);
            } else {
                os << string_square(square);
            }

            if (!board.history.empty() && (board.history.back().m.pos_new == square)) {
                os << "*";
            } else {
                os << " ";
            }
        }

        switch (rank) {
        case SIZE:
            os << B_UD << " ";
            for (index = 0; index < board.black_captures.size(); index++) {
                os << piece_symbol(board.black_captures[index]);
            }
            break;
        case 6:
            os << B_UD << " ";
            os << (board.to_move == WHITE ? "White's move: " : "Black's move: ") << board.full_moves;
            break;
        case 5:
            os << B_UD << " ";
            switch (board.get_status()) {
            case STATUS_CHECK:
                os << "Check";
                break;
            case STATUS_CHECKMATE:
                os << "Checkmate.. gg";
                break;
            case STATUS_STALEMATE:
                os << "Stalemate.. gg";
                break;
            case STATUS_DRAW:
                os << "Draw";
                break;
            }
            break;
        case 1:
            os << B_UD << " ";
            for (index = 0; index < board.white_captures.size(); index++) {
                os << piece_symbol(board.white_captures[index]);
            }
            break;
        default:
            os << B_UD;
        }
        os << endl;
    }

    os << B_UDR << B_LR << B_LR << B_LR << B_LRUD << B_LR;
    for (index = 0; index < SIZE; index++) {
        os << B_LR << B_LR;
    }
    os << B_UDL << endl;

    //file captions
    os << B_UD << " ";

    os << piece_symbol(PAWN * board.to_move) << " ";
    os << B_UD;
    os << " a b c d e f g h " << B_UD;

    //last move
    if (!board.history.empty()) {
        os << " " << string_move(board.history.back().m);
    }

    os << endl;
    //close borders
    os << B_RU << B_LR << B_LR << B_LR << B_LRU << B_LR;
    for (index = 0; index < SIZE; index++) {
        os << B_LR << B_LR;
    }
    os << B_LU;
    os << endl;
#ifdef DEBUG
    os << "wc: " << board.white_castle << " bc: " << board.black_castle;
    os << " wk: " << get_square_by_index(board.white_king) << " bk: " << get_square_by_index(board.black_king);
    os << " en: " << get_square_by_index(board.en_passant);
    os << " 50r: " << board.fifty_moves << " full: " << board.full_moves << endl;
    os << board.get_fen() << endl;
#endif
    return os;
}

#else

ostream& operator<<(ostream& os, Board& board) {
#ifdef TOURNAMENT
    os << endl << board.get_fen() << endl;
#endif

	int square;
    for (int rank = SIZE; rank > 0; rank--) {
        os << rank << " ";
        for (int file = 0; file < SIZE; file++) {
            square = (rank - 1) * NEXT_RANK + file * NEXT_FILE;
            if (board.board[square] != EMPTY) {
                os << piece_symbol(board.board[square]);
            } else {
                if ((file + rank) & 1) {
                    os << ".";
                } else {
                    os << "-";
                }
            }
            if (!board.history.empty() && board.history.back().m.pos_new
                    == square) {
                os << "*";
            } else {
                os << " ";
            }
        }

        switch (rank) {
            case SIZE:
            os << "  ";
                for (unsigned i = 0; i < board.black_captures.size(); i++) {
                    os << piece_symbol(board.black_captures[i]);
                }
                break;
            case 6:
                os << "  ";
                os << (board.to_move == WHITE ? "White's move: " : "Black's move: ") << board.full_moves;
                break;
            case 5:
                os << "  ";
                switch (board.get_status()) {
                case STATUS_CHECK:
                    os << "Check";
                    break;
                case STATUS_CHECKMATE:
                    os << "Checkmate";
                    break;
                case STATUS_STALEMATE:
                    os << "Stalemate";
                    break;
                case STATUS_DRAW:
                    os << "Draw";
                    break;
                }
                break;
            case 1:
                os << "  ";
                for (unsigned i = 0; i < board.white_captures.size(); i++) {
                    os << piece_symbol(board.white_captures[i]);
                }
                break;
        }
        os << endl;
    }
    os << "  a b c d e f g h    ";
    //last move
    if (!board.history.empty()) {
        os << " " << string_move(board.history.back().m);
    }

    os << endl;
#ifdef DEBUG
    os << "wc: " << board.white_castle << " bc: " << board.black_castle << "  en: " << get_square_by_index(board.en_passant)<< endl;
    os <<board.get_fen()<<endl;
#endif

    return os;
}

#endif
