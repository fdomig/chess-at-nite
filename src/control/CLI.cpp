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

#include "CLI.h"

CLI::CLI() {
    fen = DEFAULT_FEN;
    rotated_board = false;
    show_about();
    start();
}

CLI::CLI(int option) {
    fen = DEFAULT_FEN;
    rotated_board = false;
    show_about();
    apply_option(option);
}

CLI::~CLI() {

}

void CLI::start() {
    int user_option = -1;
    while (user_option != QUIT) {
        show_options();
        user_option = get_user_option();
        apply_option(user_option);
    }
}

void CLI::init_game(int game_type) {
    rotated_board = false;
    both_human = false;
    switch (game_type) {
        case HUMAN_VS_CPU:
            white_player = new HumanPlayer();
            black_player = new ComputerPlayer();
            break;
        case CPU_VS_HUMAN:
            white_player = new ComputerPlayer();
            black_player = new HumanPlayer();
            rotated_board = true;
            break;
        case HUMAN_VS_HUMAN:
            white_player = new HumanPlayer();
            black_player = new HumanPlayer();
            both_human = true;
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
    board = new Board(fen, rotated_board);
    game = new Game(board, white_player, black_player, both_human);
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
    cout << " Welcome to " << PROJECT_NAME << ", v" << VERSION;
    cout << " (c) 2009-2010" << endl;
    cout << "     http://chess-at-nite.googlecode.com" << endl;
}

void CLI::select_fen() {
    //lots of knights to test the algebraic notation
    fen = "k7/8/8/2N1N3/1N3N2/8/1N3N2/2N1N2K w - - 0 18";
    fen = BENCHMARK_FEN;
    Board b = Board(fen);
    cout << b << endl;
}

/**
 * Benchmark test that calculates how many nodes/second chess-at-nite searches.
 *
 * It sets the position to move 18 of "The Game of the Century"
 * Donald Byrne vs Robert James Fischer (Rosenwald Memorial Tour, Oct 17 1956)
 * More info:
 *   - http://en.wikipedia.org/wiki/The_Game_of_the_Century_%28chess%29
 *   - http://www.chessgames.com/perl/chessgame?gid=1008361
 *   - http://goo.gl/Yp0o (shortet the above game)
 */
void CLI::run_benchmark() {
    // old bench: "rq3rk1/4bppp/p1Rp1n2/8/4p3/1B2BP2/PP4PP/3Q1RK1 w - - 0 17"
    string fen = BENCHMARK_FEN;
    Board* b = new Board(fen);
    Player* p = new ComputerPlayer(false);
    p->set_board(b);

    cout << "----- The Game of the Century -----" << endl;
    cout << "Donald Byrne vs Robert J. Fischer" << endl;
    cout << "Rosenwald Memorial, Oct 17 1956" << endl;
    cout << "Watch the game: http://goo.gl/Yp0o" << endl;

    cout << *b;
    int t[3];
    int n[3];
    for (int i = 0; i < 3; i++) {
        int start = get_ms();
        p->get_move();
        t[i] = get_ms() - start;
        n[i] = b->checked_nodes;
    }
    cout << "-------- Benchmark Results --------" << endl;
    double total = 0;
    for (int i = 0; i < 3; i++) {
        double nps = (n[i] / (double) t[i]) * 1000;
        total += nps;
        cout << "     Run " << i + 1 << ": " << (int) nps << " nodes/sec"
                << endl;
    }
    cout << endl;
    cout << "   Average: " << (int) (total / 3) << " nodes/sec" << endl;
    delete p;
    delete b;
}

bool CLI::compare_found_move(string found, string should) {
    vector<string> moves;
    split(should, moves, ' ');

    for (unsigned int i = 0; i < moves.size(); i++) {
        if (found.compare(moves[i]) == 0) {
            return true;
        }
    }
    return false;
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
        cerr << "Can not run Win At Chess test: Test file '" << WAC_FILE;
        cerr << "' is missing." << endl;
        return;
    }

    vector<string> found;
    vector<string> should;
    vector<bool> results;
    vector<string> fens;
    string algebraic = "";

    int total_tested = 0;
    int total_solved = 0;

    string line = "";
    while (!file.eof()) {
        getline(file, line);
        if (line.size() < 4) {
            continue;
        }

        string cmd = "";
        cmd = line.substr(0, 4);

        if (cmd.compare("noop") == 0) {
            continue;
        } else if (cmd.compare("echo") == 0) {
            printf("%s\n", line.substr(5, line.length() - 5).c_str());
        } else if (cmd.compare("svfe") == 0) {
            string fen = "";
            fen = line.substr(5, line.length() - 5);
            fens.push_back(fen);
            Board board = Board(fen);
            Player* player = new ComputerPlayer(false);
            player->set_board(&board);
            move m = player->get_move();
            algebraic = move_to_algebraic(m, board);
            found.push_back(algebraic);
            delete player;
        } else if (cmd.compare("srch") == 0) {
            string answer = line.substr(5, line.length() - 5);
            should.push_back(answer);
            bool success = compare_found_move(algebraic, answer);
            results.push_back(success);
            total_tested++;

            if (success) {
                total_solved++;
                cout << "Test " << total_tested << " successful. ";
                cout << "Found: " << algebraic << endl;
            } else {
                cout << "Test " << total_tested << " failed. ";
                cout << "Found: " << algebraic;
                cout << ", should be: " << answer << endl;
            }
            printf("Solved %d/%d tests (%.1f%%)\n\n",
                    total_solved, total_tested,
                    (float) (total_solved) / total_tested * 100);
        }
    }

    file.close();

    cout << "---- Test Results ----" << endl;
    printf("Solved %d/%d tests (%.1f%%)\n\n",
            total_solved,
            total_tested,
            (float) (total_solved) / total_tested * 100);

    cout << "---- Failed results ----" << endl;

    if (found.size() == should.size() && fens.size() == results.size()
            && fens.size() == found.size()) {
        for (unsigned i = 0; i < results.size(); i++) {
            if (!results[i]) {
                cout << setw(3) << i + 1 << ". " << fens[i] << endl;
                cout << "     Found: " << found[i] << ", should be: " << should[i] << endl;
                cout << endl;
            }
        }
    } else {
        cerr << "Something went wrong with the results. Check the test file." << endl;
    }
}

