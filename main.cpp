#include <iostream>
#include <string>
#include <tuple>
#include "engine.hpp"
#include "random_move.hpp"
#include "tree.hpp"
#include <ctime>
#include <array>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <random>
#include <sstream>

std::random_device rd;
std::mt19937 rng(rd());

std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> opening_book;

std::string fen_to_key(const std::string& fen) {
    std::istringstream iss(fen);
    std::string fen_fields[6];
    for (int i = 0; i < 6; ++i) iss >> fen_fields[i];
    return fen_fields[0] + " " + fen_fields[1] + " " + fen_fields[2] + " " + fen_fields[3];
}

void load_opening_book(const std::string& filename) {
    std::ifstream in(filename);
    if(!in.is_open()){
        std::cerr << "could not open the file \n";
    }
    std::string line;
    std::cerr << "here\n";
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string fen_fields[6], move_pair;
        // Read 6 FEN fields
        for (int i = 0; i < 6; ++i) iss >> fen_fields[i];
        // Only use the first 4 for the key!
        std::string fen_key = fen_to_key(line);
        std::vector<std::pair<std::string, int>> moves;
        while (iss >> move_pair) {
            size_t colon = move_pair.find(':');
            if (colon != std::string::npos) {
                std::string move = move_pair.substr(0, colon);
                int count = std::stoi(move_pair.substr(colon + 1));
                moves.emplace_back(move, count);
            }
        }
        if (!moves.empty()){
            opening_book[fen_key] = moves;
        }
    }
}


std::string get_weighted_book_move(const std::vector<std::pair<std::string, int>>& moves, std::mt19937& rng) {
    int total = 0;
    for (const auto& [move, count] : moves)
        total += count;
    std::uniform_int_distribution<int> dist(1, total);
    int r = dist(rng);
    for (const auto& [move, count] : moves) {
        if (r <= count)
            return move;
        r -= count;
    }
    return moves.front().first; // fallback
}


bool get_opening_book(const std::string& fen){
    auto it = opening_book.find((fen_to_key(fen)));
    if(it != opening_book.end()){
        std::string book_move = get_weighted_book_move(it->second, rng);
        std::cout << book_move << std::endl;
        std::cerr << "got move from opening book\n";
        return true;
    }
    return false;
}

TranspositionTable tt;
int main() {
    srand(time(0));
    std::string line;
    ChessEngine engine;
    load_opening_book("C:/lines.txt");  // Your file
    while (std::getline(std::cin, line)) {
        if (line.find("position fen") == 0) {
            engine.fen = line.substr(13); // Extract FEN string
            auto [b, p, white_to_move] = engine.setPosition(engine.fen); //unpack everything that the setPos function return
            engine.board = b;
            engine.pieces = p;
            engine.white_to_move = white_to_move;
            uint64_t key = compute_zobrist_key(engine.white_to_move, engine.pieces, engine.board);
            engine.board.key = key; //store the key in the board
        } else if (line == "go") {
            std::cerr << engine.fen << "\n";
            if(!get_opening_book(engine.fen)){
                int max_depth = 4;
                node_t root = node_t(engine.white_to_move, std::array<int, 5> {-1,-1,-1,-1,-1}, engine.board, engine.pieces);
                std::pair<int, std::array<int, 5>> result = minimax(root, max_depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 1, tt);
                output_move(result.second);
                std::vector<std::array<int, 5>> l_moves = all_legal_moves(engine.white_to_move, engine.pieces, engine.board);
            }
            print_board(engine.board);
        } else if (line == "quit") {
            break;
        } else {
            std::cerr << "Unknown command: " << line << std::endl;
        }
    }

    return 0;
}



// Include your own headers
// #include "engine.hpp"

/*
int main() {
    std::vector<std::string> test_fens = {
        // Starting position
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        // Knights only
        "8/8/3qk3/3pp3/3PP3/3QK3/8/8 w - - 0 1",
        // Endgame - lone kings
        "8/8/8/8/8/8/8/4K2k w - - 0 1",
        // Stalemate
        "7k/5Q2/6K1/8/8/8/8/8 b - - 0 1",
        // Multiple pins
        "r3k2r/pppb1ppp/2np4/4p3/4P3/2NP4/PPP2PPP/R3K2R w KQkq - 0 1",
        // Castling rights only one side
        "r3k2r/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/R3K2R b KQkq - 0 1"
    };

    // This is pseudo-code: replace with your FEN->board/pieces parsing
    for (const auto& fen : test_fens) {
        // Parse FEN
        std::cout << fen << "\n";
        ChessEngine engine;
        auto [board, pieces, white_to_move] = engine.setPosition(fen);

        engine.white_to_move = white_to_move;
        engine.board = board;
        engine.pieces = pieces;
        int is_white = white_to_move; // or set manually
        const int NUM_CALLS = 10;
        int max_depth = 4;
        node_t root = node_t(engine.white_to_move, std::array<int, 5> {-1,-1,-1,-1,-1}, engine.board, engine.pieces);
        // Time the function
        auto start = std::chrono::high_resolution_clock::now();
        std::pair<int, std::array<int, 5>> result = minimax(root, max_depth, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 1);
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "FEN: " << fen << "\n";
        std::cout << "Time for " << NUM_CALLS << " calls: " << ms << " ms\n\n";
    }

    return 0;
}
*/
