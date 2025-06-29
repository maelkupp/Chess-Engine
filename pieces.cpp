#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "engine.hpp"
#include "random_move.hpp"
#include <memory>

std::vector<std::array<int, 5>> pawn_t::pseudo_legal_moves(const board_t& b, const pieces_t& pieces, int attack) {
    //will not implement en passant for now
    std::vector<std::array<int, 5>> moves;
    

    if(this->is_white){
        //white pawn
        if(this->row == 7){
            //potential promotion
            if(!b.board[this->column-1]){
                moves.push_back({this->column, this->row, this->column, this->row+1, 1}); //queen promotion 
                moves.push_back({this->column, this->row, this->column, this->row+1, 2}); //knight promotion
            }
            if(this->column < 8 && b.board[this->column] && b.board[this->column]->is_white != this->is_white){
                //diagonal right eat and promotion
                moves.push_back({this->column, this->row, this->column+1, this->row+1, 1}); 
                moves.push_back({this->column, this->row, this->column+1, this->row+1, 2});
            }
            if(this->column > 1 &&b.board[this->column - 2] && b.board[this->column - 2]->is_white != this->is_white){
                //diagonal left and promotion
                moves.push_back({this->column, this->row, this->column-1, this->row+1, 1}); 
                moves.push_back({this->column, this->row, this->column-1, this->row+1, 2});                    
            }
        }else{
            //normal move not a promotion
            if(!b.board[(7 - this->row)*8 + this->column - 1]){
                moves.push_back({this->column, this->row, this->column, this->row+1, 0}); //normal move
                if(this->row == 2 && !(b.board[32 + this->column - 1])){
                    //there is space in front of the pawn, the pawn is at rank2 and there is space two places in front
                    moves.push_back({this->column, this->row, this->column, this->row+2, 0});   
                }
            }
            
            if(this->column < 8 && b.board[(7 - this->row)*8 + this->column] && b.board[(7 - this->row)*8 + this->column]->is_white != this->is_white){
                //diagonal right eat
                moves.push_back({this->column, this->row, this->column+1, this->row+1, 0}); 
            }
            if(this->column > 1 &&b.board[(7 - this->row)*8 + this->column - 2] && b.board[(7 - this->row)*8 + this->column - 2]->is_white != this->is_white){
                //diagonal left
                moves.push_back({this->column, this->row, this->column-1, this->row+1, 0});                   
            }

            //we also check for en passant
            if(this->row == 5 && b.en_passant_row && b.en_passant_col && (this->column == b.en_passant_col + 1 || this->column == b.en_passant_col -1) && !b.board[(8 - b.en_passant_row)*8 + b.en_passant_col - 1]){
                moves.push_back({this->column, this->row, b.en_passant_col, b.en_passant_row, 0});
            }
        }
    }else{
        //black pawn
        if(this->row == 2){
            //potential promotion
            if(!b.board[56 + this->column - 1]){
                moves.push_back({this->column, this->row, this->column, this->row-1, 1}); //queen promotion 
                moves.push_back({this->column, this->row, this->column, this->row-1, 2}); //knight promotion
            }
            if(this->column < 8 && b.board[56 + this->column] && b.board[56 + this->column]->is_white != this->is_white){
                //diagonal right eat and promotion
                moves.push_back({this->column, this->row, this->column+1, this->row-1, 1}); 
                moves.push_back({this->column, this->row, this->column+1, this->row-1, 2});
            }
            if(this->column > 1 &&b.board[56 + this->column - 2] && b.board[56 + this->column - 2]->is_white != this->is_white){
                //diagonal left and promotion
                moves.push_back({this->column, this->row, this->column-1, this->row-1, 1}); 
                moves.push_back({this->column, this->row, this->column-1, this->row-1, 2});                    
            }
        }else{
            //normal move not a promotion
            if(!b.board[(9 - this->row)*8 + this->column - 1]){
                moves.push_back({this->column, this->row, this->column, this->row-1, 0}); //normal move
                if(this->row == 7 && !(b.board[24 + this->column - 1])){
                    //there is space in front of the pawn, the pawn is at rank7 and there is space two places in front
                    moves.push_back({this->column, this->row, this->column, this->row-2, 0});   
                }
            }
            
            if(this->column < 8 && b.board[(9 - this->row)*8 + this->column] && b.board[(9 - this->row)*8 + this->column]->is_white != this->is_white){
                //diagonal right eat
                moves.push_back({this->column, this->row, this->column+1, this->row-1, 0}); 
            }
            if(this->column > 1 &&b.board[(9 - this->row)*8 + this->column - 2] && b.board[(9 - this->row)*8 + this->column - 2]->is_white != this->is_white){
                //diagonal left eat
                moves.push_back({this->column, this->row, this->column-1, this->row-1, 0});                   
            }

            //we also check for en passant
            if(this->row == 4 && b.en_passant_row && b.en_passant_col && (this->column == b.en_passant_col + 1 || this->column == b.en_passant_col -1) && !b.board[(8 - b.en_passant_row)*8 + b.en_passant_col - 1]){
                moves.push_back({this->column, this->row, b.en_passant_col, b.en_passant_row, 0});
            }
        }
    }

    return moves;
}

std::vector<std::array<int, 5>> knight_t::pseudo_legal_moves(const board_t& b, const pieces_t& pieces, int attack) {
    //can make this more optimized
    std::vector<std::array<int, 5>> moves;
    const int dx[] = {2, 1, -1, -2, -2, -1, 1, 2};
    const int dy[] = {1, 2, 2, 1, -1, -2, -2, -1};

    for(int k=0; k<8; k++){
        if(this->column + dx[k] <= 8 && this->column + dx[k] >= 1 && this->row + dy[k] <= 8 && this->row+dy[k] >= 1){
            //if the knight does not go over the board
            if(!(b.board[(8 - this->row - dy[k])*8 + this->column + dx[k] - 1])){

                //if there is no piece the knight can jump there
                moves.push_back({this->column, this->row, this->column+dx[k], this->row+dy[k], 0});
            }else if(b.board[(8 - this->row - dy[k])*8 + this->column + dx[k] - 1]->is_white != this->is_white){
                //there is a piece we simply check if it is of the same colour as the knight of not
                moves.push_back({this->column, this->row, this->column+dx[k], this->row+dy[k], 0});
            }
        }
            
    }
    return moves;
}


std::vector<std::array<int, 5>> bishop_t::pseudo_legal_moves(const board_t& b, const pieces_t& pieces, int attack) {
    std::vector<std::array<int, 5>> moves;
    int upper_right = 1;
    int upper_left = 1;
    int lower_left = 1;
    int lower_right = 1;


    for(int i=1; i<8; i++){
        if(upper_right && this->column + i <= 8 && this->row + i <= 8){
            //the upper right diagonal
            if(!(b.board[(8 - this->row - i)*8 + this->column + i -1])){
                //there is no piece
                moves.push_back({this->column, this->row, this->column+i, this->row+i, 0});
            }else if(b.board[(8 - this->row - i)*8 + this->column + i -1]->is_white != this->is_white){
                //there is a piece and we just check they are not of the same colour
                moves.push_back({this->column, this->row, this->column+i, this->row+i, 0});
                upper_right = 0;
            }else{
                //there is a piece but it is the same colour thus we are blocked and cannot move in this direction any more
                upper_right = 0;
            }
        }
        if(upper_left && this->column - i >= 1 && this->row + i <= 8){
            //the upper left diagonal
            if(!(b.board[(8 - this->row - i)*8 + this->column - i -1])){
                //there is no piece
                moves.push_back({this->column, this->row, this->column-i, this->row+i, 0});
            }else if(b.board[(8 - this->row - i)*8 + this->column - i -1]->is_white != this->is_white){
                //there is a piece and we just check they are not of the same colour
                moves.push_back({this->column, this->row, this->column-i, this->row+i, 0});
                upper_left = 0;
            }else{
                //there is a piece but it is the same colour thus we are blocked and cannot move in this direction any more
                upper_left = 0;
            }
        }
        if(lower_left && this->column - i >= 1 && this->row - i >= 1){
            //the lower left diagonal
            if(!(b.board[(8 - this->row + i)*8 + this->column - i -1])){
                //there is no piece
                moves.push_back({this->column, this->row, this->column-i, this->row-i, 0});
            }else if(b.board[(8 - this->row + i)*8 + this->column - i -1]->is_white != this->is_white){
                //there is a piece and we just check they are not of the same colour
                moves.push_back({this->column, this->row, this->column-i, this->row-i, 0});
                lower_left = 0;
            }else{
                //there is a piece but it is the same colour thus we are blocked and cannot move in this direction any more
                lower_left = 0;
            }
        }
        if(lower_right && this->column + i <= 8 && this->row - i >= 1){
            //the lower right diagonal
            if(!(b.board[(8 - this->row + i)*8 + this->column + i -1])){
                //there is no piece
                moves.push_back({this->column, this->row, this->column+i, this->row-i, 0});
            }else if(b.board[(8 - this->row + i)*8 + this->column + i -1]->is_white != this->is_white){
                //there is a piece and we just check they are not of the same colour
                moves.push_back({this->column, this->row, this->column+i, this->row-i, 0});
                lower_right = 0;
            }else{
                //there is a piece but it is the same colour thus we are blocked and cannot move in this direction any more
                lower_right = 0;
            }
        }
    }

    return moves;
}


std::vector<std::array<int, 5>> rook_t::pseudo_legal_moves(const board_t& b, const pieces_t& pieces, int attack) {
    std::vector<std::array<int, 5>> moves;
    int up = 1;
    int down = 1;
    int left = 1;
    int right = 1;

    for(int i=1; i<8; i++){
        if(up && this->row + i <= 8){
            if(!(b.board[(8 - this->row - i)*8 + this->column - 1])){
                //empty position
                moves.push_back({this->column, this->row, this->column, this->row+i, 0});
            }else if(b.board[(8 - this->row - i)*8 + this->column - 1]->is_white != this->is_white){
                moves.push_back({this->column, this->row, this->column, this->row+i, 0});
                up = 0; 
            }else{
                up = 0;
            }
        }
        if(down && this->row - i >= 1){
            if(!(b.board[(8 - this->row + i)*8 + this->column - 1])){
                //empty position
                moves.push_back({this->column, this->row, this->column, this->row-i, 0});; 
            }else if(b.board[(8 - this->row + i)*8 + this->column - 1]->is_white != this->is_white){
                moves.push_back({this->column, this->row, this->column, this->row-i, 0});
                down = 0; 
            }else{
                down = 0;
            }
        }
        if(left && this->column - i >= 1){
            if(!(b.board[(8 - this->row)*8 + this->column - i - 1])){
                //empty position
                moves.push_back({this->column, this->row, this->column-i, this->row, 0});
            }else if(b.board[(8 - this->row)*8 + this->column - i - 1]->is_white != this->is_white){
                moves.push_back({this->column, this->row, this->column-i, this->row, 0});
                left = 0; 
            }else{
                left = 0;
            }
        }
        if(right && this->column + i <= 8){
            if(!(b.board[(8 - this->row)*8 + this->column + i - 1])){
                //empty position
                moves.push_back({this->column, this->row, this->column+i, this->row, 0});
            }else if(b.board[(8 - this->row)*8 + this->column + i - 1]->is_white != this->is_white){
                moves.push_back({this->column, this->row, this->column+i, this->row, 0});
                right = 0;
            }else{
                right = 0;
            }
        }
    }
    return moves;
}


std::vector<std::array<int, 5>> king_t::pseudo_legal_moves(const board_t& b, const pieces_t& p, int attack) {
    std::vector<std::array<int, 5>> moves;
    int bx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int by[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for(int k=0; k < 8; k++){
        if(row + by[k] >= 1 && row + by[k] <= 8 && column + bx[k] >= 1 && column + bx[k] <= 8){
            if(!(b.board[(8 - this->row - by[k])*8 + this->column + bx[k] -1])){
                //no piece
                moves.push_back({this->column, this->row, this->column+bx[k], this->row+by[k], 0});
            }else if(b.board[(8 - this->row - by[k])*8 + this->column + bx[k] -1]->is_white != this->is_white){
                //there is a piece but it is an enemy piece
                moves.push_back({this->column, this->row, this->column+bx[k], this->row+by[k], 0});
            }
        }
    }
    if(attack){
        if(this->is_white){
            if(b.white_king_side && !b.board[61] && !b.board[62] && std::dynamic_pointer_cast<rook_t>(b.board[63]) && !check_at_pos(this->is_white, std::vector<int>{this->column, 6, 7}, std::vector<int>{this->row,1,1},p, b)){
                //careful with the difference between the indices for b.board and the row/column number for check_at_pos
                //king still has castling rights, there is no piece in between the king and the rook and no enemy piece can touch one of the position between the
                moves.push_back({5,1,7,1});
            }
            if(b.white_queen_side && !b.board[57] && !b.board[58] && !b.board[59] && std::dynamic_pointer_cast<rook_t>(b.board[56]) && !check_at_pos(this->is_white, std::vector<int>{this->column,2,3,4}, std::vector<int>{this->row,1,1,1}, p, b)){
                moves.push_back({5,1,3,1});
            }
        }else{
            if(b.black_king_side && !b.board[5] && !b.board[6] && std::dynamic_pointer_cast<rook_t>(b.board[7]) && !check_at_pos(this->is_white, std::vector<int>{this->column,6,7}, std::vector<int>{this->row,8,8}, p, b)){
                moves.push_back({5,8,7,8});
            }
            if(b.black_queen_side && !b.board[1] && !b.board[2] && !b.board[3] && std::dynamic_pointer_cast<rook_t>(b.board[0]) && !check_at_pos(this->is_white, std::vector<int>{this->column,2,3,4}, std::vector<int>{this->row,8,8,8}, p, b)){
                moves.push_back({5,8,3,8});
            }
        }
    }


    return moves;
}

std::vector<std::array<int, 5>> queen_t::pseudo_legal_moves(const board_t& b, const pieces_t& p, int attack) {
    std::vector<std::array<int, 5>> moves;
    rook_t rook_part(this->is_white, this->column, this->row);
    bishop_t bishop_part(this->is_white, this->column, this->row);

    auto rook_moves = rook_part.pseudo_legal_moves(b, p, attack);
    auto bishop_moves = bishop_part.pseudo_legal_moves(b, p, attack);

    moves.insert(moves.end(), rook_moves.begin(), rook_moves.end());
    moves.insert(moves.end(), bishop_moves.begin(), bishop_moves.end());
    return moves;
    return moves;
}

