#include "citrus/board/Board.h"

class Position {
	public:
		Position();
		Position(Board new_board, Board::Color new_side_to_move, bool new_castling_rights[2], int new_ep_square);
		
		Board get_board() const;
		Board::Color get_side_to_move() const;
		bool get_castling_rights(Board::Color c) const;
		int get_ep_square() const;
		
	private:
		Board board;
		Board::Color side_to_move;
		bool castling_rights[2];
		int ep_square;
};
