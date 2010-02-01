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

#include "Board.h"

Board::Board(bool inversed) : inversed(inversed) {
    inititalize();
}

//templated function, that performs a memcpy of compile-time know size objects
template <class T>
void objcpy(T& dst, const T& src) {
    memcpy(&dst, &src, sizeof (T));
}

//TODO: it's not copying eeeverything.. use it at your own risk!!!
Board::Board(const Board& b) {
    objcpy(board, b.board);
    history.insert(history.end(), b.history.begin(), b.history.end());
    pgn.insert(pgn.end(), b.pgn.begin(), b.pgn.end());
    black_captures.insert(black_captures.end(), b.black_captures.begin(), b.black_captures.end());
    white_captures.insert(white_captures.end(), b.white_captures.begin(), b.white_captures.end());

    black_king = b.black_king;
    white_king = b.white_king;
    black_castle = b.black_castle;
    white_castle = b.white_castle;

    en_passant = b.en_passant;
    fifty_moves = b.fifty_moves;
    full_moves = b.full_moves;
    stop_time = b.stop_time;
    to_move = b.to_move;
    checked_nodes = b.checked_nodes;
    time_exit = b.time_exit;
    inversed = b.inversed;
    status = b.status;
    current_hash = b.current_hash;

    objcpy(hash_casteling_black, b.hash_casteling_black);
    objcpy(hash_casteling_white, b.hash_casteling_white);
    objcpy(hash_en_passant, b.hash_en_passant);
    objcpy(hash_pieces, b.hash_pieces);
    objcpy(hash_promotion, b.hash_promotion);
}

Board::Board(const string& fen, bool inversed) : inversed(inversed) {
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
    for (int square = 0; square < BOARD_SIZE; ++square) {
        board[square] = EMPTY;
    }
    to_move = WHITE;
    en_passant = NO_SQUARE;
    white_castle = CASTLE_LONG | CASTLE_SHORT;
    black_castle = CASTLE_LONG | CASTLE_SHORT;
    fifty_moves = 0;
    full_moves = 1;

    history.clear();
    pgn.clear();
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

void Board::set_inversed(int inverse) {
    inversed = inverse;
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
    fen.append(square_to_string(en_passant));
    fen.append(" ");
    fen.append(int_to_string(fifty_moves));
    fen.append(" ");
    fen.append(int_to_string(full_moves));
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
    to_move = OPPONENT(to_move);

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
    to_move = OPPONENT(to_move);

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
 *
 * !! You have to update the pgn vector if you want to have the history in PGN!
 * add_pgn();
 */
void Board::play_move(move m) {
    fake_move(m);
    if (m.content > EMPTY) {
        black_captures.push_back(m.content);
    } else if (m.content < EMPTY) {
        white_captures.push_back(m.content);
    }
}

/*
 * Because we do more than just undo if it's an actual undo by a human player
 */
bool Board::undo_move() {
    bool result = false;
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
        result = true;
    }

    //remove last pgn...
    if (!pgn.empty()) {
        pgn.pop_back();
    }
    return result;
}

void Board::add_pgn(string algebraic) {
    pgn.push_back(algebraic);
}

void Board::initialize_hash() {
    // pieces
    for (int piece = 0; piece < PIECES; piece++) {
        for (int color = 0; color < COLORS; color++) {
            for (int square = 0; square < BOARD_SIZE; square++) {
                hash_pieces[piece][color][square] = hash_rand();
            }
        }
    }
    // en passant
    for (int square = 0; square < BOARD_SIZE; square++) {
        hash_en_passant[square] = hash_rand();
    }
    // castling options
    for (int i = 0; i <= CASTLE_LONG; i++) {
        hash_casteling_black[i] = hash_rand();
        hash_casteling_white[i] = hash_rand();
    }

    // promotion pieces
    for (int piece = 0; piece < PIECES; piece++) {
        hash_promotion[piece] = hash_rand();
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
    int color = m.moved_piece > EMPTY ? 0 : 1;
    int piece = abs(m.moved_piece) - 1;
    key ^= hash_pieces[piece][color][m.pos_old];
    key ^= hash_pieces[piece][color][m.pos_new];

    if (m.content != 0) {
        key ^= hash_pieces[abs(m.content) - 1][color][m.pos_new];
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
    for (int square = 0; square < BOARD_SIZE; square++) {
        int piece = board[square];
        if (piece > EMPTY) {
            key ^= hash_pieces[piece - 1][0][square];
        } else if (piece < EMPTY) {
            key ^= hash_pieces[-piece - 1][1][square];
        }
    }
    key ^= hash_casteling_white[white_castle];
    key ^= hash_casteling_black[black_castle];
    if (to_move == BLACK) {
        key ^= hash_side;
    }
    return key;
}

#ifdef USE_HASH_TABLE

htype Board::hash_probe(int depth, int* alpha, int beta) {
    htentry* entry = hash_entry(current_hash);
    if (entry->key == current_hash) {
        if (entry->depth >= depth) {
            switch (entry->type) {
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

ostream & operator<<(ostream& os, Board& board) {
    int square;
    unsigned int index;

    int new_rank;
    int new_file;

    os << B_RD << B_LR << B_LR << B_LR << B_LRD << B_LR;

    for (index = 0; index < SIZE; index++) {
        os << B_LR << B_LR;
    }
    os << B_LD << endl;

    for (int rank = SIZE; rank > 0; rank--) {
        new_rank = rank;
        if (board.inversed) {
            new_rank = SIZE - rank + 1;
        }
        os << B_UD << " " << new_rank << " " << B_UD << " ";
        for (int file = 0; file < SIZE; file++) {
            new_file = file;
            if (board.inversed) {
                new_file = SIZE - file - 1;
            }

            square = (new_rank - 1) * NEXT_RANK + (new_file) * NEXT_FILE;
            if (board.board[square] != EMPTY) {
                os << piece_symbol(board.board[square]);
            } else {
                os << empty_square_to_string(square);
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
                if (board.inversed) {
                    for (index = 0; index < board.white_captures.size(); index++) {
                        os << piece_symbol(board.white_captures[index]);
                    }
                } else {
                    for (index = 0; index < board.black_captures.size(); index++) {
                        os << piece_symbol(board.black_captures[index]);
                    }
                }
                break;
            case 6:
                os << B_UD << " ";
                os << (board.to_move == WHITE ? "White's turn (" : "Black's turn (") << board.full_moves << ")";
                break;
            case 5:
                os << B_UD << " ";
                switch (board.get_status()) {
                    case STATUS_CHECK:
                        os << "Check..!";
                        break;
                    case STATUS_WHITE_MATES:
                        os << "White mates..! gg";
                        break;
                    case STATUS_BLACK_MATES:
                        os << "Black mates..! gg";
                        break;
                    case STATUS_WHITE_WINS:
                        os << "White wins..! gg";
                        break;
                    case STATUS_BLACK_WINS:
                        os << "Black wins..! gg";
                        break;
                    case STATUS_STALEMATE:
                        os << "Stalemate.. gg";
                        break;
                    case STATUS_DRAW:
                        os << "Draw.. gg";
                        break;
                }
                break;
            case 1:
                os << B_UD << " ";
                if (board.inversed) {
                    for (index = 0; index < board.black_captures.size(); index++) {
                        os << piece_symbol(board.black_captures[index]);
                    }
                } else {
                    for (index = 0; index < board.white_captures.size(); index++) {
                        os << piece_symbol(board.white_captures[index]);
                    }
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

    //next to move.. lower left corner
    os << B_UD << " ";
#ifdef UNICODE
    os << piece_symbol(PAWN * board.to_move);
#else
    os << " ";
#endif
    os << " " << B_UD;

    //file captions
    if (!board.inversed) {
        os << " a b c d e f g h " << B_UD;
    } else {
        os << " h g f e d c b a " << B_UD;
    }

    //last move
    if (!board.pgn.empty()) {
        os << " " << board.pgn.back();
    }

    if (!board.history.empty()) {
        os << "   " << move_to_string(board.history.back().m);
    }

    os << endl;
    //close borders
    os << B_RU << B_LR << B_LR << B_LR << B_LRU << B_LR;
    for (index = 0; index < SIZE; index++) {
        os << B_LR << B_LR;
    }
    os << B_LU << endl;
#ifdef DEBUG
    os << board.get_fen() << endl;
    os << "w_king: " << square_to_string(board.white_king);
    os << "  b_king: " << square_to_string(board.black_king) << endl;
#endif
    return os;
}
