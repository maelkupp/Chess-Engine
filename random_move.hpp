#ifndef RANDOM_MOVE_HPP
#define RANDOM_MOVE_HPP

#include <string>
#include <array>
#include <vector>
#include <tuple>
#include <utility>
#include <iostream>
#include <memory>

std::shared_ptr<king_t> find_king(int is_white, const pieces_t& pieces); //finds the king on the chess board of the colour is_white
std::vector<std::array<int, 3>> pinned_pos(int king_col, int king_row, int king_is_white, board_t board); //calculates all the pinned positions on the chessboard
std::vector<std::array<int, 5>> all_pseudo_legal_moves(int is_white, pieces_t pieces, board_t board, int attack); //computes all pseudo legal moves of the is_white pieces
std::vector<std::array<int, 5>> all_pseudo_legal_attack_moves(int is_white, piece_t pieces, board_t board); //computes all pseudo legal moves that are attack moves 
int check_at_pos(int is_white, std::vector<int> columns, std::vector<int> rows, pieces_t pieces, board_t board); //returns a boolean stating whether the position (column , row) is in check
std::vector<std::array<int, 5>> all_legal_moves(int is_white, pieces_t pieces, board_t board); //computes al legal moves of the is_white pieces
void output_move(std::array<int, 5> move); //formats the move in the UCI format

#endif