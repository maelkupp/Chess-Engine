#include <string>
#include <tuple>
#include "tree.hpp"
#include "make_move.hpp"
#include <ctime>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <limits>

std::pair<int, std::array<int, 5>> quiesce(node_t& position, int alpha, int beta){
    if(position.score >= beta){
        return std::pair<int, std::array<int, 5>> {position.score, position.move};
    }
    if(position.score > alpha){
        alpha = position.score;
    }
    int best_score = position.score;
    std::array<int, 5> best_move = position.move;
    for(int i=0; i<position.leg_moves.size(); i++){
        if(position.board.board[(8 - position.leg_moves[i][3])*8 + position.leg_moves[i][2] - 1]){
            //there is a piece where the move ends up (this is a capture)
            MoveBack moveback = make_move(position.board, position.pieces, position.leg_moves[i]);
            auto [score, _] = quiesce(node_t(!position.is_white, position.leg_moves[i],position.board, position.pieces), -beta, -alpha);
            score = -score;
            undo_move(position.board, position.pieces, moveback);

            if(score > best_score){
                best_score = score;
                best_move = position.leg_moves[i];
            }
            if (score > alpha) {
                alpha = score;
            }
            if (alpha >= beta){
                break; // Beta cutoff
                }
        }
    }
    return {best_score, best_move};
}

std::pair<int, std::array<int, 5>> minimax(node_t position, int depth, int alpha, int beta, int maximizing_player, TranspositionTable& tt){
    if(position.leg_moves.size() == 0){
        //we have no legal moves so we are in check mate, so the opponent the node above has won, we return +infinity if we are the minim player (node above is the maxim player
        if(maximizing_player){
   return std::pair<int, std::array<int,5>> {std::numeric_limits<int>::min(), position.move};
            
        }
        return std::pair<int, std::array<int, 5>> {std::numeric_limits<int>::max(), position.move};
    }

        // Probe TT
    TTEntry* entry = tt.lookup(position.board.key, depth);
    if (entry) {
        if (entry->flag == EXACT)       return {entry->score, entry->best_move};
        if (entry->flag == LOWERBOUND && entry->score >= beta)  return {entry->score, entry->best_move};
        if (entry->flag == UPPERBOUND && entry->score <= alpha) return {entry->score, entry->best_move};
        // Otherwise, continue searching as usual
    }

    if(depth == 0){
        return quiesce(position, alpha, beta); //we have reached the leaf of the tree so we return the evaluation of the board at that point
        //return {position.score, position.move};
        
    }

    //keep the original alpha in a variable for the logic
    int alphaOrig = alpha;

    if(maximizing_player){
        int maxEval = std::numeric_limits<int>::min();
        std::array<int, 5> best_move = position.leg_moves[0];
        for(int i=0; i<position.leg_moves.size(); i++){
            MoveBack moveback = make_move(position.board, position.pieces, position.leg_moves[i]); //have updated the board and the pieces
            std::pair<int, std::array<int, 5>> pair = minimax(node_t(!position.is_white, position.leg_moves[i], position.board, position.pieces), depth-1, alpha, beta, !maximizing_player, tt);
            undo_move(position.board, position.pieces, moveback);
            if(pair.first > maxEval){
                best_move = position.leg_moves[i];
                maxEval = pair.first;}
            if(pair.first > alpha){alpha = pair.first;}
            if(beta <= alpha){
                break;
            }
        }

        TTFlag flag = EXACT;
        if (maxEval <= alphaOrig) flag = UPPERBOUND;
        else if (maxEval >= beta) flag = LOWERBOUND;
        tt.store(position.board.key, maxEval, depth, flag, best_move);

        return std::pair<int, std::array<int, 5>> {maxEval, best_move};
    }else{
        int minEval = std::numeric_limits<int>::max();
        std::array<int, 5> best_move = position.leg_moves[0];
        for(int i=0; i<position.leg_moves.size(); i++){
            MoveBack moveback = make_move(position.board, position.pieces, position.leg_moves[i]); //have updated the board and the pieces
            std::pair<int, std::array<int, 5>> pair = minimax(node_t(!position.is_white, position.leg_moves[i], position.board, position.pieces), depth-1, alpha, beta, !maximizing_player, tt);
            undo_move(position.board, position.pieces, moveback);
            if(pair.first < minEval){
                best_move = position.leg_moves[i];
                minEval = pair.first;}
            if(pair.first < beta){beta = pair.first;}
            if(beta <= alpha){
                break;
            }
        }


        TTFlag flag = EXACT;
        if (minEval >= beta) flag = LOWERBOUND;
        else if (minEval <= alphaOrig) flag = UPPERBOUND;
        tt.store(position.board.key, minEval, depth, flag, best_move);

        return std::pair<int, std::array<int, 5>> {minEval, best_move};
    }
};


int score_move(int is_white, std::array<int, 5> move, board_t& b, pieces_t& p){
    MoveBack moveback = make_move(b, p, move);
    int score = simple_position_score(is_white, p);
    undo_move(b, p, moveback);
    return score;
};

bool is_capture(const std::array<int,5>& move, const board_t& board) {
    int dest_index = (8 - move[3])*8 + move[2] - 1;
    return board.board[dest_index] != nullptr;
};


uint64_t compute_zobrist_key(int is_white, pieces_t& p, board_t& board) {
    uint64_t key = 0;

    for (int i=0; i<p.pieces.size(); i++) {
        int piece_idx = p.pieces[i]->type; // 0=Pawn, 1=Knight, ..., 5=King
        int color = p.pieces[i]->is_white;    // 0=White, 1=Black
        key ^= board.zobristPiece[piece_idx][color][(8 - p.pieces[i]->row)*8 + p.pieces[i]->column - 1];
    
    }

    // Castling rights (encode to [0..15] as in Polyglot)
    int castling = (board.white_king_side << 0)
                 | (board.white_queen_side << 1)
                 | (board.black_king_side << 2)
                 | (board.black_queen_side << 3);
    key ^= board.zobristCaslting[castling];

    // En passant (if any)

    // Side to move
    if (!is_white) {
        key ^= board.zobristSide;
    }

    return key;
};

TTEntry* TranspositionTable::lookup(uint64_t key, int depth){
    TTEntry &entry = table[key % TT_SIZE];
    if (entry.key == key && entry.depth >= depth) {
        return &entry;
    }
    return nullptr;
};

void TranspositionTable::store(uint64_t key, int score, int depth, TTFlag flag, std::array<int, 5> best_move){
    TTEntry &entry = table[key % TT_SIZE];
    // Simple replacement: always store, or only replace if depth is greater
    if (entry.key != key || depth >= entry.depth) {
        entry = {key, score, depth, flag, best_move};
    }
};