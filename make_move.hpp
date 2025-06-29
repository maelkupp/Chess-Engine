#ifndef MAKE_MOVE_HPP
#define MAKE_MOVE_HPP

struct MoveBack{
    int from_col, from_row;
    int to_col, to_row;
    int promotion;
    int en_passant_col;
    int en_passant_row;
    int white_king_castle, white_queen_castle; //ints telling us what the castling rights of the board is
    int black_king_castle, black_queen_castle;
    int white_king_castled, white_queen_castled; //ints to tell us if the king caslted in the move
    int black_king_castled, black_queen_castled;
    std::shared_ptr<piece_t> captured_piece;
    std::shared_ptr<piece_t> moved_piece; //the moved_piece also capture the to be promoted pawn so I do not need to add such an attribute
    std::shared_ptr<piece_t> promoted_piece;
    std::shared_ptr<piece_t> en_passant_pawn; //the pawn that gets eaten due to the en passant
};

MoveBack make_move(board_t& board, pieces_t& pieces, std::array<int, 5> move);
void undo_move(board_t& board, pieces_t& pieces, const MoveBack& moveback);
#endif