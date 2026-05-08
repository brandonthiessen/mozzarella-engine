#include "move_generation.h"
#include "perft.h"
#include "utils.h"
#include "search.h"
#include "uci.h"
#include <vector>
#include <sstream>


void uci_loop() {
    std::string line;
    Position pos;
    MovePicker mp = MovePicker(64);

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name Mozzarella" << std::endl;
            std::cout << "id author Brandon Thiessen" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (line == "isready") {
            std::cout << "readyok" << std::endl;
        }
        else if (line.rfind("position", 0) == 0) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;  // read "position"
            iss >> token;  // read "startpos" or "fen"

            if (token == "startpos") {
                pos = Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            }
            else if (token == "fen") {
                // Read the FEN string (6 space-separated parts)
                std::string fen_str;
                for (int i = 0; i < 6; i++) {
                    if (iss >> token) {
                        if (i > 0) fen_str += " ";
                        fen_str += token;
                    }
                }
                pos = Position(fen_str);
            }

            // Parse and apply moves if present
            if (iss >> token && token == "moves") {
                std::string move_str;
                while (iss >> move_str) {
                    uint32_t m = parse_move(pos, move_str);
                    pos.move(m);
                }
            }
        }
        else if (line.rfind("go", 0) == 0) {

            int wtime = -1, btime = -1;
            int winc = 0, binc = 0; // currently unused
            int movetime = -1;
            int depth = -1;

            std::istringstream iss(line);
            std::string token;
            iss >> token; // read "go"

            // Perft mode options via "go perft N"
            bool perft_mode = false;
            int perft_depth = -1;

            // Read in all values from "go" command
            while (iss >> token) {
                if (token == "perft") {
                    perft_mode = true;
                    iss >> perft_depth;
                }
                if (token == "wtime") iss >> wtime;
                else if (token == "btime") iss >> btime;
                else if (token == "winc") iss >> winc;
                else if (token == "binc") iss >> binc;
                else if (token == "movetime") iss >> movetime;
                else if (token == "depth") iss >> depth;
            }

            if (perft_mode) {
                std::vector<uint32_t> moves = generate_legal_moves(&pos);

                long total_nodes = 0;

                for (uint32_t m: moves) {
                    pos.move(m);
                    long nodes = perft(&pos, perft_depth - 1);
                    total_nodes += nodes;
                    pos.unmove(m);

                    std::cout << move_to_string(m) << ": " << nodes << std::endl;
                }

                std::cout << "\nNodes searched: " << total_nodes << std::endl;
                continue;
            }

            int remaining = (pos.player_to_move == Player::WHITE) ? wtime : btime;

            int time_budget = (movetime != -1)
                ? movetime
                : std::max(100, remaining > 0 ? remaining / 50 : 100);

            uint32_t best_move;

            // Search either to a fixed depth depending on "go" request
            if (depth != -1) {
                best_move = mp.find_best_move_fixed_depth(&pos, depth);
            } else {
                best_move = mp.find_best_move(&pos, time_budget);
            }

            // Make the move on the internal board
            pos.move(best_move);

            // Output the move to CLI
            std::cout << "bestmove " << move_to_string(best_move) << std::endl;

        }
        else if (line == "quit") {
            break;
        }
    }
}


int main() {
    std::ios::sync_with_stdio(false);
    uci_loop();
    return 0;
}