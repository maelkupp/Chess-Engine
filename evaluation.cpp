#include <iostream>
#include <string>
#include <tuple>
#include "engine.hpp"
#include "random_move.hpp"
#include <ctime>
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <cstdlib>

std::array<int, 10> counting_pieces(const pieces_t& p){
    std::array<int, 10> count = {0,0,0,0,0,0,0,0,0,0};
    //0:wP 1:wR 2:wN 3:wB 4:wQ 5:bP 6:bR 7:bN 8:bB 9:bQ
    for(int i=0; i<p.pieces.size(); i++)
    if(p.pieces[i]->is_white){
        if(std::dynamic_pointer_cast<pawn_t>(p.pieces[i])){count[0]++;
        }else if(std::dynamic_pointer_cast<rook_t>(p.pieces[i])){count[1]++;
        }else if(std::dynamic_pointer_cast<knight_t>(p.pieces[i])){count[2]++;
        }else if(std::dynamic_pointer_cast<bishop_t>(p.pieces[i])){count[3]++;
        }else if(std::dynamic_pointer_cast<queen_t>(p.pieces[i])){count[4]++;}
    }else{
        if(std::dynamic_pointer_cast<pawn_t>(p.pieces[i])){count[5]++;
        }else if(std::dynamic_pointer_cast<rook_t>(p.pieces[i])){count[6]++;
        }else if(std::dynamic_pointer_cast<knight_t>(p.pieces[i])){count[7]++;
        }else if(std::dynamic_pointer_cast<bishop_t>(p.pieces[i])){count[8]++;
        }else if(std::dynamic_pointer_cast<queen_t>(p.pieces[i])){count[9]++;}
    }
    return count;
}

int is_endgame(const std::array<int, 10>& count){
    // Indices: 0:wP 1:wR 2:wN 3:wB 4:wQ 5:bP 6:bR 7:bN 8:bB 9:bQ

    // Fastest check: both queens are gone
    if (count[4] == 0 && count[9] == 0)
        return 1;

    int white_non_pawn = count[1] + count[2] + count[3] + count[4];
    int black_non_pawn = count[6] + count[7] + count[8] + count[9];

    // If only one queen left and no other major/minor pieces on that side, and the other has no queen
    if ((count[4] == 1 && (count[1] + count[2] + count[3]) == 0 && count[9] == 0) ||
        (count[9] == 1 && (count[6] + count[7] + count[8]) == 0 && count[4] == 0))
        return 1;

    // only two or more minor pieces
    if (white_non_pawn + black_non_pawn <= 2)
        return 1;

    // Otherwise, it's not the endgame
    return 0;
}

int king_table_middle_game[64] = {
    19970, 19960, 19960, 19950, 19950, 19960, 19960, 19970,
    19970, 19960, 19960, 19950, 19950, 19960, 19960, 19970,
    19970, 19960, 19960, 19950, 19950, 19960, 19960, 19970,
    19970, 19960, 19960, 19950, 19950, 19960, 19960, 19970,
    19980, 19970, 19970, 19960, 19960, 19970, 19970, 19980,
    19990, 19980, 19980, 19980, 19980, 19980, 19980, 19990,
    20020, 20020, 20000, 20000, 20000, 20000, 20020, 20020,
    20020, 20030, 20010, 20000, 20000, 20010, 20030, 20020
};

int king_table_end_game[64] = {
    19950, 19960, 19970, 19980, 19980, 19970, 19960, 19950,
    19970, 19980, 19990, 20000, 20000, 19990, 19980, 19970,
    19970, 19990, 20020, 20030, 20030, 20020, 19990, 19970,
    19970, 19990, 20030, 20040, 20040, 20030, 19990, 19970,
    19970, 19990, 20030, 20040, 20040, 20030, 19990, 19970,
    19970, 19990, 20020, 20030, 20030, 20020, 19990, 19970,
    19970, 19970, 20000, 20000, 20000, 20000, 19970, 19970,
    19950, 19970, 19970, 19970, 19970, 19970, 19970, 19950
};

int position_score(int endgame, int is_white, const pieces_t& p){
    int pos_score = 0;
    for(int i=0; i<p.pieces.size(); i++){
        if(endgame){
            if(std::dynamic_pointer_cast<king_t>(p.pieces[i])){
                if(p.pieces[i]->is_white == is_white){pos_score += king_table_end_game[(8-p.pieces[i]->row)*8 + p.pieces[i]->column - 1];
                }else{pos_score -= king_table_end_game[(8-p.pieces[i]->row)*8 + p.pieces[i]->column - 1];}
            }else{
                if(p.pieces[i]->is_white == is_white){pos_score += p.pieces[i]->table_value[(8-p.pieces[i]->row)*8 + p.pieces[i]->column - 1];
                }else{pos_score -= p.pieces[i]->table_value[(8-p.pieces[i]->row)*8 + p.pieces[i]->column - 1];}
            }

        }else{
            if(std::dynamic_pointer_cast<king_t>(p.pieces[i])){
                if(p.pieces[i]->is_white == is_white){pos_score += king_table_middle_game[(8-p.pieces[i]->row)*8 + p.pieces[i]->column - 1];
                }else{pos_score -= king_table_middle_game[(8-p.pieces[i]->row)*8 + p.pieces[i]->column - 1];}
            }else{
                if(p.pieces[i]->is_white == is_white){pos_score += p.pieces[i]->table_value[(8-p.pieces[i]->row)*8 + p.pieces[i]->column - 1];
                }else{pos_score -= p.pieces[i]->table_value[(8-p.pieces[i]->row)*8 + p.pieces[i]->column - 1];}
            }
        }

    }
    return pos_score;
}

int simple_position_score(int is_white, const pieces_t& p){
    int pos_score = 0;
    for(int i=0; i < p.pieces.size(); i++){
        if(p.pieces[i]->is_white == is_white){
            pos_score += p.pieces[i]->table_value[(8 - p.pieces[i]->row)*8 + p.pieces[i]->column - 1];
        }else{
            pos_score -= p.pieces[i]->table_value[(8 - p.pieces[i]->row)*8 + p.pieces[i]->column - 1];
        }
    }
    return pos_score;
}

int king_safety(int is_white, const pieces_t& p, const board_t& b){
    int score=0;
    std::shared_ptr<piece_t> king = find_king(is_white, p);
    int col = king->column;
    int row = king->row;
    if(king->is_white){
        if(king->row == 1 && (col == 7 || col == 3)){
            //king is in the castles position
            score += 20;
            if(b.board[48+king->column-1] && std::dynamic_pointer_cast<pawn_t>(b.board[48+king->column-1]) && b.board[48+king->column-1]->is_white == is_white){
                //pawn in front
                score += 10;
            }else{score -=10;}
            if(b.board[48+king->column] && std::dynamic_pointer_cast<pawn_t>(b.board[48+king->column]) && b.board[48+king->column]->is_white == is_white){
                //pawn front right
                score += 10;
            }else{score -=10;}
            if(b.board[48+king->column-2] && std::dynamic_pointer_cast<pawn_t>(b.board[48+king->column-2]) && b.board[48+king->column-2]->is_white == is_white){
                //pawn front left
                score += 10;
            }else{score -=10;}
            
        }
    }else{
        if(king->row == 8 && (king->column==7 || king->column == 3)){
            //king in caslted position
            score += 20;
            if(b.board[8+king->column-1] && std::dynamic_pointer_cast<pawn_t>(b.board[8+king->column-1]) && b.board[8+king->column-1]->is_white == is_white){
                //pawn in front
                score += 10;
            }else{score -=10;}
            if(b.board[8+king->column] && std::dynamic_pointer_cast<pawn_t>(b.board[8+king->column]) && b.board[8+king->column]->is_white == is_white){
                //pawn front right
                score += 10;
            }else{score -=10;}
            if(b.board[8+king->column-2] && std::dynamic_pointer_cast<pawn_t>(b.board[8+king->column-2]) && b.board[8+king->column-2]->is_white == is_white){
                //pawn front left
                score += 10;
            }else{score -=10;}
        }
    }

    const int offsets[12][2] = {
    {+1,+1}, {+1,0}, {+1,-1},
    {0,+1},  {0,-1}, {0,0},
    {-1,+1}, {-1,0}, {-1,-1},
    {+1,+2}, {0,+2}, {-1,+2}
    };

    int king_ring[12][2];
    int row_sign = king->is_white ? +1 : -1;

    for(int i=0; i<12; ++i) {
        king_ring[i][0] = offsets[i][0];
        king_ring[i][1] = row_sign * offsets[i][1];
    }
    
    for(int i=0; i<12; i++){
        if(king->column + king_ring[i][0] >= 1 && king->column + king_ring[i][0] <= 8 && king->row + king_ring[i][1] >= 1 && king->row + king_ring[i][1] <= 8){
            //correctly in the board
            std::vector<int> pos_col = {king->column + king_ring[i][0]};
            std::vector<int> pos_row = {king->row    + king_ring[i][1]};
            if (check_at_pos(is_white, pos_col, pos_row, p, b)) { score -= 8;}
        }
    }
    return score;
}

int eval_board(int is_white, const board_t& b, const pieces_t& p){
    int eval = 0;
    std::array<int, 10> piece_count = counting_pieces(p);
    int endgame = is_endgame(piece_count);
    eval += position_score(endgame, is_white, p);
    if(!endgame){
        eval += king_safety(is_white, p, b);
        eval -= king_safety(!is_white, p, b);
    }
    return eval;
}

