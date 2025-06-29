#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <cstdlib>
#include <cmath>
#include "engine.hpp"
#include "random_move.hpp"
#include "make_move.hpp"
#include <memory>

MoveBack make_move(board_t& b, pieces_t& p, std::array<int, 5> move){
    if(move[0] == 3 && move[1]==5 && move[2]==2 && move[3]==4){
        print_board(b);
    }
    MoveBack moveback;
    moveback.from_col = move[0];
    moveback.from_row = move[1];
    moveback.to_col   = move[2];
    moveback.to_row   = move[3];
    moveback.promotion = move[4];

    // Save castling rights before any changes
    int old_castling = b.white_king_side | (b.white_queen_side << 1) |
                       (b.black_king_side << 2) | (b.black_queen_side << 3);

    moveback.en_passant_col = b.en_passant_col;
    moveback.en_passant_row = b.en_passant_row;

    moveback.white_king_castle  = b.white_king_side;
    moveback.white_queen_castle = b.white_queen_side;
    moveback.black_king_castle  = b.black_king_side;
    moveback.black_queen_castle = b.black_queen_side;

    moveback.white_king_castled  = 0;
    moveback.white_queen_castled = 0;
    moveback.black_king_castled  = 0;
    moveback.black_queen_castled = 0;

    // Piece references
    moveback.moved_piece = b.board[(8 - move[1]) * 8 + move[0] - 1];
    moveback.captured_piece = b.board[(8 - move[3]) * 8 + move[2] - 1];

    // === Zobrist: Remove captured piece, if any
    if (moveback.captured_piece) {
        b.key ^= b.zobristPiece[moveback.captured_piece->type][moveback.captured_piece->is_white]
                              [(8 - moveback.to_row) * 8 + moveback.to_col - 1];
    }

    //handle en passant
    if(moveback.to_col == moveback.en_passant_col && moveback.to_row == moveback.en_passant_row && moveback.moved_piece->type == 0){
        //an en passant occured we need to remove the pawn that got eaten from the board and from the pieces struct
        print_board(b);
        if(moveback.en_passant_row == 6){
            //the pawn getting en passant eaten is black
            b.board[(9 - moveback.en_passant_row)*8 + moveback.en_passant_col - 1]->print_piece();
            moveback.en_passant_pawn = b.board[(9 - moveback.en_passant_row)*8 + moveback.en_passant_col - 1]; // the pawn is found at this position
            b.board[(9 - moveback.en_passant_row)*8 + moveback.en_passant_col - 1] = nullptr; //the pawn got eaten so we remove it from the board
        }else{
            //the pawn getting en passent eaten is white
            b.board[(7 - moveback.en_passant_row)*8 + moveback.en_passant_col - 1]->print_piece();
            moveback.en_passant_pawn = b.board[(7 - moveback.en_passant_row)*8 + moveback.en_passant_col - 1]; // the pawn is found at this position
            b.board[(7 - moveback.en_passant_row)*8 + moveback.en_passant_col - 1] = nullptr; //the pawn got eaten so we remove it from the board
        }
    }else{
        moveback.en_passant_pawn = nullptr;
    }

        // Remove old en passant from Zobrist if any
    if (b.en_passant_col) {
        b.key ^= b.zobristEnPassant[b.en_passant_col - 1];
    }

    // Clear en passant
    b.en_passant_col = 0;
    b.en_passant_row = 0;
    // After move: if this move is a two-square pawn push, set new en passant square
    if (moveback.moved_piece->type == 0) { // pawn
        if (abs(moveback.to_row - moveback.from_row) == 2) {
            b.en_passant_col = moveback.from_col;
            b.en_passant_row = (moveback.from_row + moveback.to_row) / 2;
        }
    }
    if(b.en_passant_col){
        // Add new en passant to Zobrist if any
        b.key ^= b.zobristEnPassant[b.en_passant_col - 1];
    }
    // === Zobrist: Remove moved piece from source
    b.key ^= b.zobristPiece[moveback.moved_piece->type][moveback.moved_piece->is_white]
                          [(8 - moveback.from_row) * 8 + moveback.from_col - 1];
    // === Zobrist: Add moved piece to destination
    b.key ^= b.zobristPiece[moveback.moved_piece->type][moveback.moved_piece->is_white]
                          [(8 - moveback.to_row) * 8 + moveback.to_col - 1];
    // === Promotion logic
    if (moveback.promotion) {
        // Remove pawn at destination
        b.key ^= b.zobristPiece[moveback.moved_piece->type][moveback.moved_piece->is_white]
                              [(8 - moveback.to_row) * 8 + moveback.to_col - 1];
        // Add promoted piece at destination
        int promoted_type = (moveback.promotion == 1) ? 4 : 1; // 4: Queen, 1: Knight
        b.key ^= b.zobristPiece[promoted_type][moveback.moved_piece->is_white]
                              [(8 - moveback.to_row) * 8 + moveback.to_col - 1];

        // Create promoted piece
        if (moveback.promotion == 1) {
            moveback.promoted_piece = std::make_shared<queen_t>(moveback.moved_piece->is_white, move[2], move[3]);
        } else {
            moveback.promoted_piece = std::make_shared<knight_t>(moveback.moved_piece->is_white, move[2], move[3]);
        }
        b.board[(8 - move[3]) * 8 + move[2] - 1] = moveback.promoted_piece;
        b.board[(8 - move[1]) * 8 + move[0] - 1] = nullptr;
        // Update p.pieces
        for (size_t i = 0; i < p.pieces.size(); ++i) {
            if (p.pieces[i] == moveback.moved_piece) {
                p.pieces[i] = moveback.promoted_piece;
                break;
            }
        }
        // Remove captured piece from vector if needed
        if (moveback.captured_piece) {
            for (size_t i = 0; i < p.pieces.size(); ++i) {
                if (p.pieces[i] == moveback.captured_piece) {
                    p.pieces[i] = p.pieces.back();
                    p.pieces.pop_back();
                    break;
                }
            }
        }
    } else {
        // Not a promotion: move piece normally
        moveback.moved_piece->column = move[2];
        moveback.moved_piece->row = move[3];

        // --- Castling logic ---
        // King move: update castling rights and possibly rook movement (castling)
        if (std::dynamic_pointer_cast<king_t>(moveback.moved_piece)) {
            if (moveback.moved_piece->is_white) {
                b.white_king_side = 0;
                b.white_queen_side = 0;
            } else {
                b.black_king_side = 0;
                b.black_queen_side = 0;
            }
            // Castling moves for white and black
            if (move[0]==5 && move[1]==1 && move[2]==7 && move[3]==1) { // White king-side
                moveback.white_king_castled = 1;
                b.board[61] = b.board[63]; b.board[61]->column = 6; b.board[63] = nullptr;
            } else if (move[0]==5 && move[1]==1 && move[2]==3 && move[3]==1) { // White queen-side
                moveback.white_queen_castled = 1;
                b.board[59] = b.board[56]; b.board[59]->column = 4; b.board[56] = nullptr;
            } else if (move[0]==5 && move[1]==8 && move[2]==7 && move[3]==8) { // Black king-side
                moveback.black_king_castled = 1;
                b.board[5] = b.board[7]; b.board[5]->column = 6; b.board[7] = nullptr;
            } else if (move[0]==5 && move[1]==8 && move[2]==3 && move[3]==8) { // Black queen-side
                moveback.black_queen_castled = 1;
                b.board[3] = b.board[0]; b.board[3]->column = 4; b.board[0] = nullptr;
            }
        }
        // Rook move: update castling rights if rook moves from home
        if (std::dynamic_pointer_cast<rook_t>(moveback.moved_piece)) {
            if (move[0]==1) {
                if (move[1]==1) b.white_queen_side = 0;
                if (move[1]==8) b.black_queen_side = 0;
            } else if (move[0]==8) {
                if (move[1]==1) b.white_king_side = 0;
                if (move[1]==8) b.black_king_side = 0;
            }
        }
        // Normal move on board
        b.board[(8 - move[3]) * 8 + move[2] - 1] = moveback.moved_piece;
        b.board[(8 - move[1]) * 8 + move[0] - 1] = nullptr;
        // Remove captured piece from vector if needed
        if (moveback.captured_piece) {
            for (size_t i = 0; i < p.pieces.size(); ++i) {
                if (p.pieces[i] == moveback.captured_piece) {
                    p.pieces[i] = p.pieces.back();
                    p.pieces.pop_back();
                    break;
                }
            }
        }else if(moveback.en_passant_pawn){
            //remove the pawn from the pieces_t struct that got eaten due to the en passant
            for (size_t i = 0; i < p.pieces.size(); ++i) {
                if (p.pieces[i] == moveback.en_passant_pawn) {
                    p.pieces[i] = p.pieces.back();
                    p.pieces.pop_back();
                    break;
                }
            }
        }
    }
    // === Zobrist: Castling rights, AFTER rights updated
    int new_castling = b.white_king_side | (b.white_queen_side << 1) |
                       (b.black_king_side << 2) | (b.black_queen_side << 3);
    if (old_castling != new_castling) {
        b.key ^= b.zobristCaslting[old_castling];
        b.key ^= b.zobristCaslting[new_castling];
    }

    // === Zobrist: Side to move
    b.key ^= b.zobristSide;
    return moveback;
}

void undo_move(board_t& b, pieces_t& p, const MoveBack& moveback){
    // Save castling rights before restoration
    int old_castling = b.white_king_side | (b.white_queen_side << 1) |
                       (b.black_king_side << 2) | (b.black_queen_side << 3);

    // === Zobrist: Undo side to move
    b.key ^= b.zobristSide;

    // === Promotions
    if (moveback.promoted_piece) {
        // Remove promoted piece at destination
        b.key ^= b.zobristPiece[moveback.promoted_piece->type][moveback.moved_piece->is_white]
                              [(8 - moveback.to_row) * 8 + moveback.to_col - 1];
        // Add pawn back at destination
        b.key ^= b.zobristPiece[moveback.moved_piece->type][moveback.moved_piece->is_white]
                              [(8 - moveback.to_row) * 8 + moveback.to_col - 1];
    }

    // --- En Passant Zobrist restore ---
    if (b.en_passant_col && b.en_passant_row) {
        b.key ^= b.zobristEnPassant[b.en_passant_col - 1];
    }

    b.en_passant_col = moveback.en_passant_col;
    b.en_passant_row = moveback.en_passant_row;

    if (b.en_passant_col && b.en_passant_row) {
        b.key ^= b.zobristEnPassant[b.en_passant_col - 1];
    }


    // === Undo move of moved piece (if not promotion)
    if (!moveback.promoted_piece) {
        // Remove moved piece from destination
        b.key ^= b.zobristPiece[moveback.moved_piece->type][moveback.moved_piece->is_white]
                              [(8 - moveback.to_row) * 8 + moveback.to_col - 1];
        // Add moved piece back to source
        b.key ^= b.zobristPiece[moveback.moved_piece->type][moveback.moved_piece->is_white]
                              [(8 - moveback.from_row) * 8 + moveback.from_col - 1];
    }

    // === Restore captured piece, if any
    if (moveback.captured_piece) {
        b.key ^= b.zobristPiece[moveback.captured_piece->type][moveback.captured_piece->is_white]
                              [(8 - moveback.to_row) * 8 + moveback.to_col - 1];
    }

    // === Restore castling rights
    b.white_king_side  = moveback.white_king_castle;
    b.white_queen_side = moveback.white_queen_castle;
    b.black_king_side  = moveback.black_king_castle;
    b.black_queen_side = moveback.black_queen_castle;
    int new_castling = b.white_king_side | (b.white_queen_side << 1) |
                       (b.black_king_side << 2) | (b.black_queen_side << 3);
    if (old_castling != new_castling) {
        b.key ^= b.zobristCaslting[old_castling];
        b.key ^= b.zobristCaslting[new_castling];
    }

    // === Restore piece locations
    moveback.moved_piece->column = moveback.from_col;
    moveback.moved_piece->row = moveback.from_row;

    b.board[(8 - moveback.from_row) * 8 + moveback.from_col - 1] = moveback.moved_piece;
    b.board[(8 - moveback.to_row) * 8 + moveback.to_col - 1] = moveback.captured_piece;

    // Restore en passant captured pawn, if any
    if (moveback.en_passant_pawn) {
        if(moveback.en_passant_pawn->is_white){
            b.board[(8 - moveback.en_passant_pawn->row) * 8 + moveback.en_passant_pawn->column - 1] = moveback.en_passant_pawn;
        }else{
            b.board[(8 - moveback.en_passant_pawn->row) * 8 + moveback.en_passant_pawn->column - 1] = moveback.en_passant_pawn;
        }
        
        p.pieces.push_back(moveback.en_passant_pawn);
        print_board(b);
    }

    // === Handle castling rook undo
    if (moveback.white_king_castled) {
        b.board[63] = b.board[61]; b.board[63]->column = 8; b.board[61] = nullptr;
    } else if (moveback.white_queen_castled) {
        b.board[56] = b.board[59]; b.board[56]->column = 1; b.board[59] = nullptr;
    } else if (moveback.black_king_castled) {
        b.board[7] = b.board[5]; b.board[7]->column = 8; b.board[5] = nullptr;
    } else if (moveback.black_queen_castled) {
        b.board[0] = b.board[3]; b.board[0]->column = 1; b.board[3] = nullptr;
    }

    // === Restore promotion (pieces vector)
    if (moveback.promoted_piece) {
        // Remove promoted piece from pieces
        for (size_t i = 0; i < p.pieces.size(); ++i) {
            if (p.pieces[i] == moveback.promoted_piece) {
                p.pieces[i] = p.pieces.back();
                p.pieces.pop_back();
                break;
            }
        }
        // Add pawn back to pieces
        p.pieces.push_back(moveback.moved_piece);
    }

    // === Restore captured piece to pieces
    if (moveback.captured_piece) {
        p.pieces.push_back(moveback.captured_piece);
    }
}