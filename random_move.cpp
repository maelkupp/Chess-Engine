#include <iostream>
#include <string>
#include <tuple>
#include <array>
#include <memory>
#include <utility>
#include <stdexcept>
#include "engine.hpp"
#include "random_move.hpp"
#include "make_move.hpp"

//file that handles the whole move generation

std::shared_ptr<king_t> find_king(int is_white, const pieces_t& p){
    //find the king on the chess board of colour is white, if does not find a king raise an error for debugging
    int found_king = 0;
    std::shared_ptr<king_t> king;

    //finding the king
    for(auto& piece: p.pieces){
        if(std::dynamic_pointer_cast<king_t>(piece) && piece->is_white == is_white){
            king = std::dynamic_pointer_cast<king_t>(piece);
            found_king = 1;
            break; 
        }
    }

    if(!found_king){
        std::cerr << "No king found on the chessboard";
    }
    return king;
}
std::vector<std::array<int, 5>> all_pseudo_legal_moves(int is_white, pieces_t p, board_t b, int attack){
    std::vector<std::array<int, 5>> all_pseudo_legal_moves;
    std::vector<std::array<int, 5>> plegal_moves;
    for(int i=0; i<size(p.pieces); i++){
        //iterating over all the active pieces in the chessboard
        if(p.pieces[i]->is_white == is_white){
            //only get the plegal moves of the pieces we are interested in
            //pass the attack parameter stating if we are calling this function from the 1st time or in the check_at_pos function
            plegal_moves = p.pieces[i]->pseudo_legal_moves(b, p, attack);

            all_pseudo_legal_moves.insert(all_pseudo_legal_moves.end(), plegal_moves.begin(), plegal_moves.end());
        }
    }
    return all_pseudo_legal_moves;
}



int check_at_pos(int is_white, std::vector<int> columns, std::vector<int> rows, pieces_t p, board_t b){
    if(columns.size() != rows.size()){
        std::cerr << "the length of the columns and rows are no equal";
        return -1;
    }

    int column;
    int row;
    int dir[8][2] = {{0,1}, {0,-1}, {1,0}, {-1,0}, {1,1}, {-1,-1}, {-1,1}, {1,-1}}; //all possible directions for the x_ray scan, third element indicates the direction (straight or diagonal)
    for(int i=0; i<columns.size(); i++){
        column = columns[i];
        row = rows[i];
        int column_index;
        int row_index;
        for(int j=0; j<8; j++){
            for(int k=1; k<8; k++){
                column_index = column + dir[j][0]*k;
                row_index = row + dir[j][1]*k;
                if(column_index < 1 || column_index > 8 || row_index < 1 || row_index > 8){
                    //we are out of the board so we need to move onto the next direction
                    break;
                }else{
                    //indices are in the board
                    if(b.board[(8 - row_index)*8 + column_index - 1]){
                        std::shared_ptr<piece_t> piece = b.board[(8 - row_index)*8 + column_index - 1];
                        if(piece->is_white != is_white){
                            //have enemy piece
                            if(k == 1){
                                //we are 1 blocks away
                                if(j<=3){
                                    //use the index with which we iterate of the dir array to determine if it is a straight lane or a diagonal lane
                                    if(std::dynamic_pointer_cast<rook_t>(piece) || std::dynamic_pointer_cast<king_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)){
                                        return 1;
                                    }else{
                                        break;
                                    }
                                }else{
                                    //diagonal moves
                                    if(is_white){
                                        if(piece->row < row){
                                            //cannot be a pawn attack
                                            if(std::dynamic_pointer_cast<bishop_t>(piece) || std::dynamic_pointer_cast<king_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)){
                                                return 1;
                                            }else{
                                                break;
                                            }
                                        }else{
                                            if(std::dynamic_pointer_cast<pawn_t>(piece) || std::dynamic_pointer_cast<bishop_t>(piece)
                                            || std::dynamic_pointer_cast<king_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)){

                                                return 1;
                                            }else{
                                                break;
                                            }
                                        }
                                    }else{
                                        //black
                                        if(piece->row > row){
                                            //cannot be a pawn attack
                                            if(std::dynamic_pointer_cast<bishop_t>(piece) || std::dynamic_pointer_cast<king_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)){
                                                return 1;
                                            }else{
                                                break;
                                            }
                                        }else{
                                            if(std::dynamic_pointer_cast<pawn_t>(piece) || std::dynamic_pointer_cast<bishop_t>(piece)
                                            || std::dynamic_pointer_cast<king_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)){
                                                return 1;
                                            }else{
                                                break;
                                            }
                                        }
                                    }
                                }
                            }else{
                                //can disregard pawns and kings since we are more than one block away
                                if(j<=3){
                                    //straight moves
                                    if(std::dynamic_pointer_cast<rook_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)){
                                        return 1;
                                    }else{
                                        break;
                                    }
                                }else{
                                    //diagonal move
                                    if(std::dynamic_pointer_cast<bishop_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)){
                                        return 1;
                                    }else{
                                        break;
                                    }
                                }  
                            }
                        }else{
                            //we have a friendly piece blocking opponent pieces in that direction
                            break;
                        }

                    }

                }
            }
        }

        //performed the straight line x-ray in each direction to determine if the position is in check, remains to check for knight attacks
        int dx[8] = {-1,-1,1,1,-2,-2,2,2};
        int dy[8] = {-2,2,-2,2,-1,1,-1,1};
        for(int k=0; k<8; k++){
            //iterate over each knight move
            column_index = column + dx[k];
            row_index = row + dy[k];
            if(row_index >= 1 && row_index <= 8 && column_index >= 1 && column_index <= 8){
                //we are in the board
                std::shared_ptr piece = b.board[(8 - row_index)*8 + column_index - 1];
                if(piece && piece->is_white != is_white && std::dynamic_pointer_cast<knight_t>(piece)){
                    return 1;
                }
            }
        }

    }

    return 0; // we did not find a single check thus we can return 0
}




std::vector<std::array<int, 3>> pinned_pos(int king_col, int king_row, int king_is_white, board_t b){
    std::vector<std::array<int, 3>> pinned;
    int dir[8][3] = {{0,1,0}, {0,-1, 1}, {1,0, 2}, {-1,0, 3}, {1,1,4}, {-1,-1,5}, {1,-1,6}, {-1,1,7}}; // x direction and they y direction (col, row) and then index of direction
    int kc = king_col - 1;
    int kr = 8 - king_row;
    for(const auto& d: dir){
        //iterating over all the possible directions
        int found_friend = 0;
        int friend_col, friend_row;
        for(int k=1; k<8; k++){
            //computing the position we are looking at
            int col_i = kc + d[0]*k; //i for index as they are only used for indexing
            int row_i = kr - d[1]*k;
            if(col_i < 0 || col_i > 7 || row_i < 0 || row_i > 7){break;}
            auto piece = b.board[row_i*8 + col_i];
            if(piece){
                if(piece->is_white == king_is_white){
                    //found a piece of the same colour as the king
                    if(found_friend){
                        //had already found a friend in the same line so there is no pin
                        break;
                    }
                    found_friend = 1; //set found_friend to true
                    friend_col = king_col + d[0]*k;
                    friend_row = king_row + d[1]*k;
                }else{
                    //found enemy piece
                    if(!found_friend){
                        //we could be in check here but there cannot be a pin so we exit the loop for this direction
                        break;
                    }else{
                        //have found a friend, need to check if the slider is of the corresponding type to cause a pin
                        if(d[0] == 0 || d[1] == 0){
                            //straight line
                            if(std::dynamic_pointer_cast<queen_t>(piece) || std::dynamic_pointer_cast<rook_t>(piece)){
                                //found a pin
                                std::array<int, 3> pos = {friend_col, friend_row, d[2]};
                                pinned.push_back(pos);
                                break;
                            }
                        }else{
                            //diagonal
                            if(std::dynamic_pointer_cast<queen_t>(piece) || std::dynamic_pointer_cast<bishop_t>(piece)){
                                //found a pin
                                std::array<int, 3> pos = {friend_col, friend_row, d[2]};
                                pinned.push_back(pos);
                                break;
                            }
                        }
                    }
                }
                
            }
        }
    }
    return pinned;
}

std::vector<std::array<int, 5>> all_legal_moves(int is_white, pieces_t p, board_t b){
    std::vector<std::array<int, 5>> legal_moves;
    std::shared_ptr<king_t> king = find_king(is_white, p);
    std::vector<std::array<int, 3>> pinned_pieces = pinned_pos(king->column, king->row, king->is_white, b); //probably need to add the direction of the pin in the vector too
    //when I have the direction I can then keep only the moves that stay in the direction
    std::vector<std::array<int, 5>> plegal_moves = all_pseudo_legal_moves(is_white, p, b, 1);

    if(check_at_pos(is_white, std::vector<int>{king->column}, std::vector<int>{king->row}, p, b)){
        //king is in check
        for(const auto& plegal_move: plegal_moves){
            MoveBack moveback = make_move(b, p, plegal_move);
            if(!check_at_pos(is_white, std::vector<int>{king->column}, std::vector<int>{king->row}, p, b)){
                legal_moves.push_back(plegal_move);
            }
            undo_move(b, p, moveback);
        }
    }else{
        //king is not in check
        for(const auto& plegal_move: plegal_moves){
        int king_move = 0;
        if(plegal_move[0] == king->column && plegal_move[1] == king->row){
            king_move = 1;
                MoveBack moveback = make_move(b, p, plegal_move);
                //found a move of the king, need to directly check if the end position of the move results in being checked need to perform the move
                if(!check_at_pos(is_white, std::vector<int>{plegal_move[2]}, std::vector<int>{plegal_move[3]}, p, b)){
                    //there is no check at the position where the king wants to move
                    //std::cerr << "move: " << plegal_move[0]<<plegal_move[1]<<plegal_move[2]<<plegal_move[3]<< " no check at the position where the king is going \n";
                    legal_moves.push_back(plegal_move);
                }
                undo_move(b, p, moveback);

            }

        int pinned = 0;
        if(!king_move){
            for(const auto& pinned_pos: pinned_pieces){
                if(pinned_pos[0] == plegal_move[0] && pinned_pos[1] == plegal_move[1]){
                    pinned = 1;
                    //this is a pinned piece so we can only add the moves that are in the line of the pin
                    if((pinned_pos[2] == 0|| pinned_pos[2] == 1) && plegal_move[2] == plegal_move[0] && plegal_move[1] != plegal_move[3]){
                        // move in the vertical pin line
                        //std::cerr << "move: " << plegal_move[0]<<plegal_move[1]<<plegal_move[2]<<plegal_move[3]<< "move in vertical pin lane \n";
                        legal_moves.push_back(plegal_move);
                        break;
                    }else if((pinned_pos[2] == 2 || pinned_pos[2] == 3) && plegal_move[0] != plegal_move[2] && plegal_move[1] == plegal_move[3]){
                        //move in the horizontal pin line
                        //std::cerr << "move: " << plegal_move[0]<<plegal_move[1]<<plegal_move[2]<<plegal_move[3]<< "move in horizontal pin lane \n";
                        legal_moves.push_back(plegal_move);
                        break;
                    }else if((pinned_pos[2]==4||pinned_pos[2]==5) && (plegal_move[0]-plegal_move[1]) == (plegal_move[2]-plegal_move[3])){
                        //move in the Id pin line
                        //std::cerr << "move: " << plegal_move[0]<<plegal_move[1]<<plegal_move[2]<<plegal_move[3]<< "move in Id pin lane \n";
                        legal_moves.push_back(plegal_move);
                        break;
                    }else if((pinned_pos[2]==6||pinned_pos[2]==7) && (plegal_move[0]+plegal_move[1]) == (plegal_move[2]+plegal_move[3])){
                        //move in the -Id pin line
                        //std::cerr << "move: " << plegal_move[0]<<plegal_move[1]<<plegal_move[2]<<plegal_move[3]<< "move in -Id pin lane \n";
                        legal_moves.push_back(plegal_move);
                        break;
                    }
                }
            }
            if(!pinned){
                //if not pinned
                //std::cerr << "move: " << plegal_move[0]<<plegal_move[1]<<plegal_move[2]<<plegal_move[3] << " not a pinned move\n";
                legal_moves.push_back(plegal_move);
            }
        }
    }
    }
    
    return legal_moves;
}

void output_move(std::array<int, 5> move){
    char file_from = 'a' + (move[0] - 1);
    char rank_from = '1' + (move[1] - 1);
    char file_to = 'a' + (move[2] - 1);
    char rank_to = '1' + (move[3] - 1);
    std::cout << file_from << rank_from << file_to << rank_to;
    // For promotion:
    if(move[4] != 0) {
        char prom;
        if(move[4]==1) prom = 'q'; // queen
        else if(move[4]==2) prom = 'n'; // knight
        else if(move[4]==3) prom = 'r'; // rook
        else if(move[4]==4) prom = 'b'; // bishop
        std::cout << prom;
    }
    std::cout << std::endl;
}



/*
int check_at_pos(int is_white, std::vector<int> columns, std::vector<int> rows, pieces_t p, board_t b) {
    if (columns.size() != rows.size()) {
        std::cerr << "the length of the columns and rows are not equal";
        return -1;
    }

    // Directions for rook/bishop/queen sliding
    int dir[8][2] = {
        {0,1},   // N
        {0,-1},  // S
        {1,0},   // E
        {-1,0},  // W
        {1,1},   // NE
        {-1,-1}, // SW
        {-1,1},  // NW
        {1,-1}   // SE
    };

    for (int idx = 0; idx < columns.size(); idx++) {
        int target_col = columns[idx];
        int target_row = rows[idx];

        // 1. Sliding attacks (rook, bishop, queen)
        for (int j = 0; j < 8; j++) {
            for (int k = 1; k < 8; k++) {
                int scan_col = target_col + dir[j][0]*k;
                int scan_row = target_row + dir[j][1]*k;
                if (scan_col < 1 || scan_col > 8 || scan_row < 1 || scan_row > 8) {
                    break; // off board
                }
                auto piece = b.board[8 - scan_row][scan_col - 1];
                if (piece) {
                    if (piece->is_white != is_white) {
                        // Enemy piece found
                        if (k == 1) {
                            // First square: check for adjacent sliding piece
                            if (j <= 3) { // straight directions
                                if (std::dynamic_pointer_cast<rook_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)) return 1;
                            } else { // diagonal directions
                                if (std::dynamic_pointer_cast<bishop_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)) return 1;
                            }
                        } else {
                            // Further along the ray: only sliding pieces matter
                            if (j <= 3) { // straight
                                if (std::dynamic_pointer_cast<rook_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)) return 1;
                            } else { // diagonal
                                if (std::dynamic_pointer_cast<bishop_t>(piece) || std::dynamic_pointer_cast<queen_t>(piece)) return 1;
                            }
                        }
                    }
                    // Blocked by any piece (friend or enemy)
                    break;
                }
            }
        }

        // 2. Knight attacks
        int dx[8] = {-1,-1,1,1,-2,-2,2,2};
        int dy[8] = {-2,2,-2,2,-1,1,-1,1};
        for (int i = 0; i < 8; i++) {
            int knight_col = target_col + dx[i];
            int knight_row = target_row + dy[i];
            if (knight_col >= 1 && knight_col <= 8 && knight_row >= 1 && knight_row <= 8) {
                auto piece = b.board[8 - knight_row][knight_col - 1];
                if (piece && piece->is_white != is_white && std::dynamic_pointer_cast<knight_t>(piece)) {
                    return 1;
                }
            }
        }

        // 3. Pawn attacks (only diagonal forward squares for correct color)
        if (is_white) {
            // Black pawns attack diagonally down
            int pawn_row = target_row + 1;
            if (pawn_row <= 8) {
                for (int dc : {-1, 1}) {
                    int pawn_col = target_col + dc;
                    if (pawn_col >= 1 && pawn_col <= 8) {
                        auto piece = b.board[8 - pawn_row][pawn_col - 1];
                        if (piece && !piece->is_white && std::dynamic_pointer_cast<pawn_t>(piece)) {
                            return 1;
                        }
                    }
                }
            }
        } else {
            // White pawns attack diagonally up
            int pawn_row = target_row - 1;
            if (pawn_row >= 1) {
                for (int dc : {-1, 1}) {
                    int pawn_col = target_col + dc;
                    if (pawn_col >= 1 && pawn_col <= 8) {
                        auto piece = b.board[8 - pawn_row][pawn_col - 1];
                        if (piece && !piece->is_white && std::dynamic_pointer_cast<pawn_t>(piece)) {
                            return 1;
                        }
                    }
                }
            }
        }

        // 4. King attacks (8 surrounding squares)
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                int king_row = target_row + dr;
                int king_col = target_col + dc;
                if (king_row >= 1 && king_row <= 8 && king_col >= 1 && king_col <= 8) {
                    auto piece = b.board[8 - king_row][king_col - 1];
                    if (piece && piece->is_white != is_white && std::dynamic_pointer_cast<king_t>(piece)) {
                        return 1;
                    }
                }
            }
        }
    }

    // No attackers found
    return 0;
}
*/



/*
int check_at_pos(int is_white, std::vector<int> columns, std::vector<int> rows, pieces_t p, board_t b){
    std::vector<std::array<int, 5>> plegal_moves = all_pseudo_legal_moves(!is_white, p, b, 0); // get all the pseudo legal moves of the opponent and set attack to false
    //setting attack to false so that we do not look at the king castling moves which would cause infinite loop
    int pos_in_check = 0;
    for(auto& move: plegal_moves){
        //std::cerr << "check at pos verif: column: " << column << " move[2]: " << move[2] << " row: " << row << " move[3]: " << move[3] << "\n";
        for(int i=0; i< columns.size(); i++){
            if(columns[i] == (move[2]) && rows[i] == (move[3])){
                //position (column[i] row[i]) is under attack as an opponents piece can eat it
                pos_in_check = 1;
                break;
            }
        }

    }
    return pos_in_check;
}
*/