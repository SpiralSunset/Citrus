#include "citrus/board/Position.h"
#include <string>

class Fen {
	public:
		std::string pos_to_str(Position pos);
		Position str_to_pos(std::string str);
	private:
		char piece_to_char(Board::Piece p);
		Board::PieceType char_to_piece_type(char c);
};
