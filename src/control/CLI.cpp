/*
 * This file is part of the chess-at-nite project.
 *
 * Copyright (c) 2009 by
 *   Franziskus Domig
 *   Panayiotis Lipiridis
 *   Radoslav Petrik
 *   Thai Gia Tuong
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "CLI.h"
#include "../model/Game.h"
#include "../model/Board.h"
#include "../player/ComputerPlayer.h"
#include "../player/HumanPlayer.h"

CLI::CLI() {
    fen = DEFAULT_FEN;
    user_option = -1;
    show_about();
    start();
}

CLI::CLI(int option) {
    fen = DEFAULT_FEN;
    show_about();
    apply_option(option);
}

CLI::~CLI() {

}

void CLI::start() {
    while (user_option != QUIT) {
        show_options();
        user_option = get_user_option();
        apply_option(user_option);
    }
}

void CLI::init_game(int game_type) {
    switch (game_type) {
    case HUMAN_VS_CPU:
        white_player = new HumanPlayer();
        black_player = new ComputerPlayer();
        break;
    case CPU_VS_HUMAN:
        white_player = new ComputerPlayer();
        black_player = new HumanPlayer();
        break;
    case HUMAN_VS_HUMAN:
        white_player = new HumanPlayer();
        black_player = new HumanPlayer();
        break;
    case CPU_VS_CPU:
        white_player = new ComputerPlayer();
        black_player = new ComputerPlayer();
        break;
    }
}

void CLI::apply_option(int option) {
    switch (option) {
    case HUMAN_VS_CPU:
    case CPU_VS_HUMAN:
    case HUMAN_VS_HUMAN:
    case CPU_VS_CPU:
        init_game(option);
        start_game();
        end_game();
        break;
    case SELECT_FEN:
        select_fen();
        break;
    case SHOW_HELP:
        print_help();
        break;
    case BENCHMARK:
        run_benchmark();
        break;
    case WAC:
        run_wac_test();
        break;
    case QUIT:
        cout << "Thanks for playing...!! Have fun.." << endl;
        break;
    }
}

void CLI::show_options() {
    cout << "-----------------------------------" << endl;

    cout << "   1. Human vs Computer" << endl;
    cout << "   2. Computer vs Human" << endl;
    cout << "   3. Human vs Human" << endl;
    cout << "   4. Computer vs Computer" << endl;
    cout << "   5. Load fen" << endl;
    cout << "   6. Help (Moves/Commands)" << endl;
    cout << "   7. Run Benchmark" << endl;
    cout << "   8. Win At Chess Test" << endl;
    cout << "-----------------------------------" << endl;
    cout << "   0. Quit" << endl;
    cout << "-----------------------------------" << endl;
}

string CLI::get_line() {
    string text;
    cin >> text;
    return text;
}

int CLI::get_user_option() {
    int result = -1;
    string temp;
    cout << "Select something: ";
    cin >> temp;

    if (temp.size() > 0 && temp[0] >= '0' && temp[0] <= '9') {
        result = temp[0] - '0';
    }
    return result;
}

void CLI::start_game() {
    board = new Board(fen);
    game = new Game(board, white_player, black_player);
    game->start_game();
}

void CLI::end_game() {
    delete board;
    delete game;
    delete white_player;
    delete black_player;
    fen = DEFAULT_FEN;
}

void CLI::show_about() {
    cout << " Welcome to ChessAtNite, v" << VERSION << " written by:" << endl;
    cout << "  - Franziskus Domig" << endl;
    cout << "  - Panayiotis Lipiridis" << endl;
    cout << "  - Radoslav Petrik" << endl;
    cout << "  - Thai Gia Tuong" << endl;
}

void CLI::select_fen() {
    fen = "3r2k1/R4p2/p2n3p/2pP2pP/2P2KP1/3B3r/P2R4/8 w - - 0 44";// en passant can escape from check, but it doesn't
    fen = DEFAULT_FEN;
    fen = "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1";
    Board b = Board(fen);
    cout << b << endl;
}

/* bench: This is a little benchmark code that calculates how many
 nodes per second TSCP searches.
 It sets the position to move 17 of Bobby Fischer vs. J. Sherwin,
 New Jersey State Open Championship, 9/2/1957.
 Then it searches five ply three times. It calculates nodes per
 second from the best time. */
void CLI::run_benchmark() {
    string fen = "rq3rk1/4bppp/p1Rp1n2/8/4p3/1B2BP2/PP4PP/3Q1RK1 w - - 0 17";
    Board* b = new Board(fen);
    Player* p = new ComputerPlayer();
    p->set_board(b);

    int t[3];
    int n[3];
    for (int i = 0; i < 3; i++) {
        int start = get_ms();
        p->get_move();
        t[i] = get_ms() - start;
        n[i] = b->checked_nodes;
    }
    cout << "-----------------------------------" << endl;
    cout << "Benchmark Results:" << endl;
    double total = 0;
    for (int i = 0; i < 3; i++) {
        double nps = (n[i] / (double) t[i]) * 1000;
        total += nps;
        cout << i + 1 << ": nodes/second: " << (int) nps << endl;
    }

    cout << "average: " << (int) (total / 3) << " nodes/second" << endl;
    delete p;
    delete b;
}

/*
 * Testsuite "Win at Chess"
 *
 * 300 Board positions which should be found a path to checkmate.
 *
 */
void CLI::run_wac_test() {
    ifstream file;
    file.open(WAC_FILE);
    if (!file) {
        cout << "Can not run Win At Chess test: Test file '" << WAC_FILE;
        cout << "' is missing." << endl;
        return;
    }

    vector<string> found;
    vector<string> should;

    string line = "";
    while (!file.eof()) {

        getline(file, line);
        if (line.size() < 4)
            continue;

        string cmd = "";
        cmd = line.substr(0, 4);

        if (cmd.compare("noop") == 0) {
            continue;
        } else if (cmd.compare("echo") == 0) {
            printf("%s\n", line.substr(5, line.length() - 5).c_str());
        } else if (cmd.compare("svfe") == 0) {
            string fen = "";
            fen = line.substr(5, line.length() - 5);
            Board b = Board(fen);
            Player* p = new ComputerPlayer();
            p->set_board(&b);
            move m = p->get_move();
            cout << "Found: " << string_move_simple(m) << endl;
            found.push_back(string_move_simple(m));
            delete p;
        } else if (cmd.compare("srch") == 0) {
            cout << "Should be: " << line.substr(5, line.length() - 5) << endl;
            should.push_back(line.substr(5, line.length() - 5));
        }
    }

    file.close();

    cout << "Test Results:" << endl;
    cout << "Found     Should" << endl;
    for (unsigned i = 0; i < should.size() && found.size(); i++) {
        cout << found[i] << "    " << should[i] << endl;
    }
}

