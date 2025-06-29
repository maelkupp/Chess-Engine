#ifndef TREE_HPP
#define TREE_HPP
#include "engine.hpp"
#include "evaluation.hpp"
#include "random_move.hpp"
#include <algorithm>


struct node_t{
    int score; // the score/evaluation of the board at this node
    std::array<int, 5> move; // the move that made the tree get to this node
    int is_white; //it is white to move if true it is black to move if false
    board_t board; //board at the node, same board given to the evalutaion function that gives integer score
    pieces_t pieces; //corresponding pieces vector
    std::vector<std::array<int, 5>> leg_moves; //the possible legal moves from this position

    node_t(int is_white, const std::array<int, 5>& move, board_t& board, pieces_t& pieces)
        : is_white(is_white), move(move), board(board), pieces(pieces)
    {   
        score = eval_board(is_white, board, pieces);
        leg_moves = all_legal_moves(is_white, pieces, board); //get all legal moves
        
        std::sort(leg_moves.begin(), leg_moves.end(),
            [&](const auto& a, const auto& b) {
                return is_capture(a, board) > is_capture(b, board); //sort the legal moves if they are a capture or not, can greatly increase the speed of the search
            }); 
    }
};


enum TTFlag { EXACT, LOWERBOUND, UPPERBOUND};

struct TTEntry{
    uint64_t key;
    int score;
    int depth;
    TTFlag flag; //exact, loswerbound or upperbound to provide additional logic in the minimax tree search
    std::array<int, 5> best_move;
};


const int TT_SIZE = 1 << 20; // 1M entries

struct TranspositionTable {
    TTEntry table[TT_SIZE];

    // Lookup returns a pointer to the entry, or nullptr if not found or depth too shallow
    TTEntry* lookup(uint64_t key, int depth);
    void store(uint64_t key, int score, int depth, TTFlag flag, std::array<int, 5> best_move);
};


bool is_capture(const std::array<int, 5>& move, const board_t& board);
int score_move(int is_white, std::array<int, 5> move, board_t& board, pieces_t& pieces);
std::pair<int, std::array<int, 5>> minimax(node_t position, int depth, int alpha, int beta, int maximizing_player, TranspositionTable& tt);
std::pair<int, std::array<int, 5>> quiesce(node_t& position, int alpha, int beta);
uint64_t compute_zobrist_key(int is_white, pieces_t& pieces, board_t& board);
#endif