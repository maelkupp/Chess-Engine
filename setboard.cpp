#include <string>
#include <array>
#include <vector>
#include <tuple>
#include "engine.hpp"
#include <iostream>
#include <memory>

/*
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
*/

std::tuple<board_t, pieces_t, int> ChessEngine::setPosition(const std::string& fen) {

    //variables we will return from the function
    board_t board;
    pieces_t pieces;
    int white_to_move = 0; //which colour will move initiate it at 0 for simplicity in the logic we will use later on
    int row = 7; //starts off with the highest rank
    int column = 0;
    int space_counter = 0;
    for(int i=0; i<fen.size(); i++){
        if(space_counter == 0){
        int piece_column = column + 1;
        int piece_row = row + 1;

        std::shared_ptr<piece_t> piece = nullptr;
        if (fen[i] == 'p')      piece = std::make_shared<pawn_t>(0, piece_column, piece_row);
        else if (fen[i] == 'P') piece = std::make_shared<pawn_t>(1, piece_column, piece_row);

        //black
        else if (fen[i] == 'r') piece = std::make_shared<rook_t>(0, piece_column, piece_row);
        else if (fen[i] == 'n') piece = std::make_shared<knight_t>(0, piece_column, piece_row);
        else if (fen[i] == 'b') piece = std::make_shared<bishop_t>(0, piece_column, piece_row);
        else if (fen[i] == 'q') piece = std::make_shared<queen_t>(0, piece_column, piece_row);
        else if (fen[i] == 'k') piece = std::make_shared<king_t>(0, piece_column, piece_row);
        //white
        else if (fen[i] == 'R') piece = std::make_shared<rook_t>(1, piece_column, piece_row);
        else if (fen[i] == 'N') piece = std::make_shared<knight_t>(1, piece_column, piece_row);
        else if (fen[i] == 'B') piece = std::make_shared<bishop_t>(1, piece_column, piece_row);
        else if (fen[i] == 'Q') piece = std::make_shared<queen_t>(1, piece_column, piece_row);
        else if (fen[i] == 'K') piece = std::make_shared<king_t>(1, piece_column, piece_row);

        if(piece){
            //if it is not simply a piece generated from the vanilla constructor
            pieces.pieces.push_back(piece);
            board.board[(7-row)*8 + column] = piece; //update both board representations if there is a piece to add
            column += 1; //increment the column counter by 1
        }

        if(fen[i] == '/'){
            row -= 1;
            column = 0;
        }; //have finished the row so we reset the column counter and increment the row counter

        if(isdigit(fen[i])){column += fen[i] - '0';}

        }


        if(space_counter == 1){
            if(fen[i] == 'w'){
                white_to_move = 1;
            }
        };

        if(space_counter == 2){
            if(fen[i] == 'K'){board.white_king_side = 1;}
            if(fen[i] == 'Q'){board.white_queen_side = 1;}
            if(fen[i] == 'k'){board.black_king_side = 1;}
            if(fen[i] == 'q'){board.black_queen_side = 1;}
        };

        if(space_counter == 3 && fen[i] != '-'){
            // if we are in the en passant section of the fen string and we have a digit in the next string then there is an en passant pos
            int col = fen[i] - 'a' + 1; // 'a' = 0, 'b' = 1, etc
            int row = fen[i+1] - '0'; // '1' = 1, '2' = 2, etc
            board.en_passant_col = col;
            board.en_passant_row = row;
        };

        //for now I dont care about the half or full moves
        if(fen[i] == ' '){space_counter ++;} //increment the space counter if we encounter a space


    }

    std::tuple<board_t, pieces_t, int> tuple{board, pieces, white_to_move};
    return tuple;
}



/*
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
*/