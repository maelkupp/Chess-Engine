#ifndef EVALUATION_HPP
#define EVALUATION_HPP


#include "engine.hpp"

int eval_board(int is_white, const board_t& board, const pieces_t& pieces);
int position_score(int endgame, int is_white, const pieces_t& p);
std::array<int, 10> counting_pieces(const pieces_t& p);
int is_endgame(const std::array<int, 10>& piece_count);
int king_safety(int is_white, const pieces_t& p, const board_t& b); //the higher the value the safer the king,
int simple_position_score(int is_white, const pieces_t& pieces);
//only looks at the king safety of the king with colour is_white
#endif