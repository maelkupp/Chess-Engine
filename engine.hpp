#ifndef CHESS_ENGINE_HPP
#define CHESS_ENGINE_HPP

#include <string>
#include <array>
#include <vector>
#include <tuple>
#include<iostream>
#include <memory>
#include <unordered_map>


struct board_t;
struct pieces_t;

extern int white_pawn_table_value[64];
extern int black_pawn_table_value[64];
extern int knight_table_value[64];
extern int bishop_table_value[64];
extern int rook_table_value[64];
extern int queen_table_value[64];
extern int king_table_value[64];
extern int king_table_middle_game[64];
extern int king_table_end_game[64];
extern uint64_t POLYGOT_RANDOM_ARRAY[781];

struct piece_t {
    const int* table_value; //used in the evaluation function
    int is_white; // attribute is true=1 if piece is white, false=0 otherwise
    int row;
    int column; //row and and column of piece in format a1 -> 11
    int type;

    virtual std::vector<std::array<int, 5>> pseudo_legal_moves(const board_t& board, const pieces_t& pieces, int attack){
        return {};
    };
    virtual void print_piece(){
        std::cout << " . ";
    };

    void display_piece(){
        //function for debugging purposes:
        print_piece();
        std::cerr << "column: " << column << " row: " << row << "\n";
    }
    piece_t(): is_white(0), row(-1), column(-1) {}; //creating a default constructor
    piece_t(int is_white, int column, int row){
        this->is_white = is_white;
        this->column = column;
        this->row = row;
    };
};

//representing our board with two structures, one 8 by 8 structure, another a simple vector with all alive/active pieces
struct board_t{
  std::vector<std::shared_ptr<piece_t>> board; //8 x 8 array of pointers to pieces to keep the polymorphism of the classes intact
  int en_passant_row;
  int en_passant_col;
  int black_king_side;
  int black_queen_side;
  int white_king_side;
  int white_queen_side;
  uint64_t key;
  uint64_t zobristPiece[6][2][64];
  uint64_t zobristCaslting[16];
  uint64_t zobristEnPassant[8];
  uint64_t zobristSide;
  board_t():
    board(64, nullptr),
    en_passant_col(0),
    en_passant_row(0),
    black_king_side(0),
    black_queen_side(0),
    white_king_side(0),
    white_queen_side(0) {
        
        int idx = 0;
        for(int piece=0; piece<6; piece++){
            for(int color=0; color<2; color++){
                for(int sq=0; sq<64; sq++){
                    zobristPiece[piece][color][sq] = POLYGOT_RANDOM_ARRAY[idx++];
                }
            }
        }
        for(int i=0; i<16; i++){
            zobristCaslting[i] = POLYGOT_RANDOM_ARRAY[idx++];
        }
        for(int i=0; i<8; i++){
            zobristEnPassant[i] = POLYGOT_RANDOM_ARRAY[idx++];
        }
        zobristSide = POLYGOT_RANDOM_ARRAY[idx++];
    }
  
};

struct pieces_t{
    std::vector<std::shared_ptr<piece_t>> pieces; //just an array of all the alive pieces
};

struct ChessEngine {
    std::tuple<board_t, pieces_t, int> setPosition(const std::string& fen); 
    std::string fen; //fen string representation of the current board used for opening table
    board_t board;
    pieces_t pieces;
    int white_to_move;
};

struct pawn_t : public piece_t {
    std::vector<std::array<int, 5>> pseudo_legal_moves(const board_t& board, const pieces_t& pieces, int attack) override; //determines all the pseudo legal moves of the piece (not taking into account being pinned)
    void print_piece() override;
    pawn_t(int is_white, int column, int row): piece_t(is_white, column, row){
        if(is_white){
            this->table_value = white_pawn_table_value;
        }else{
            this->table_value = black_pawn_table_value;
        }
        this->type = 0;
    };
};

struct knight_t : public piece_t {
    std::vector<std::array<int, 5>> pseudo_legal_moves(const board_t& board, const pieces_t& pieces, int attack) override;
    void print_piece() override;
    knight_t(int is_white, int column, int row): piece_t(is_white, column, row){
        this->table_value = knight_table_value;
        this->type = 1;
    };
};

struct bishop_t : public piece_t {
    std::vector<std::array<int, 5>> pseudo_legal_moves(const board_t& board, const pieces_t& pieces, int attack) override;
    void print_piece() override;

    bishop_t(int is_white, int column, int row): piece_t(is_white, column, row){
        this->table_value = bishop_table_value;
        this->type = 2;
    };
};

struct rook_t : public piece_t {
    std::vector<std::array<int, 5>> pseudo_legal_moves(const board_t& board, const pieces_t& pieces, int attack) override;
    void print_piece() override;

    rook_t(int is_white, int column, int row): piece_t(is_white, column, row){
        this->table_value = rook_table_value;
        this->type = 3;
    };
};

struct king_t : public piece_t {
    std::vector<std::array<int, 5>> pseudo_legal_moves(const board_t& board, const pieces_t& pieces, int attack);
    void print_piece() override;

    king_t(int is_white, int column, int row): piece_t(is_white, column, row){
        this->table_value = king_table_value;
        this->type = 5;
    };
};

struct queen_t : public piece_t {
    std::vector<std::array<int, 5>> pseudo_legal_moves(const board_t& board, const pieces_t& pieces, int attack) override;
    void print_piece() override;

    queen_t(int is_white, int column, int row): piece_t(is_white, column, row){
        this->table_value = queen_table_value;
        this->type = 4;
    };
};
void print_board(const board_t& board);
std::shared_ptr<piece_t> find_piece_at(int column, int row, pieces_t pieces); // if there is a piece at that given point return it otherwise return null pointer


#endif