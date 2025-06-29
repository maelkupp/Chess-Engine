#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "engine.hpp"
#include <memory>


void print_board(const board_t& board){ //always pass board as a reference because we cannot copy the unique pointers of the pieces
    std::string s = "----";
    for(int k=0; k<8; k++){std::cerr << s;} //top of the chessboard
    std::cerr << "\n";
    for(int i=0; i<64; i++){
            std::cerr << "|";
            if(board.board[i]){
                board.board[i]->print_piece(); //call the print_piece function associated to each piece
            }else{
                std::cerr << " . ";
            }
            
        
            if(i%8==7){std::cerr << "| " << 8 - i/8 << "\n";}
    }

    for(int i=0; i<8; i++){std::cerr << s;} //bottom of the chessboard
    std::cerr << "\n a/1 b/2 c/3 d/4 e/5 f/6 g/7 h/8 \n";
}


void pawn_t::print_piece(){
    if(this->is_white){ //'this' is a pointer
        std::cerr << " P ";
    }else{
        std::cerr << " p ";
    }
    
}

void rook_t::print_piece(){
    if(this->is_white){ //'this' is a pointer
        std::cerr << " R ";
    }else{
        std::cerr << " r ";
    }
    
}

void bishop_t::print_piece(){
    if(this->is_white){ //'this' is a pointer
        std::cerr << " B ";
    }else{
        std::cerr << " b ";
    }
    
}

void knight_t::print_piece(){
    if(this->is_white){ //'this' is a pointer
        std::cerr << " N ";
    }else{
        std::cerr << " n ";
    }
    
}

void queen_t::print_piece(){
    if(this->is_white){ //'this' is a pointer
        std::cerr << " Q ";
    }else{
        std::cerr << " q ";
    }
    
}

void king_t::print_piece(){
    if(this->is_white){ //'this' is a pointer
        std::cerr << " K ";
    }else{
        std::cerr << " k ";
    }
    
}

std::shared_ptr<piece_t> find_piece_at(int column, int row, pieces_t p){
    for(int i=0; i<p.pieces.size(); i++){
        if(p.pieces[i]->column == column && p.pieces[i]->row == row){
            return p.pieces[i];
        }
    }

    return nullptr;
}