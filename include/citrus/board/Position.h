#include "citrus/board/Board.h"

class Position {
	public:
		enum class CastlingRight : uint8_t {
			wKing = 0,
			wQueen = 1,
			bKing = 2,
			bQueen = 3
		};
	
		Position();
		Position(Board new_board, Board::Color new_side_to_move = Board::Color::cWhite, std::array<bool, 4> new_castling_rights = {true, true, true, true}, int new_ep_square = -1, int new_halfmove_clock = 0, int new_fullmove_counter = 0);
		
		Board get_board() const;
		Board::Color get_side_to_move() const;
		bool get_castling_right(CastlingRight cr) const;
		int get_ep_square() const;
		int get_halfmove_clock() const;
		int get_fullmove_counter() const;
		
	private:
		Board board;
		Board::Color side_to_move;
		std::array<bool, 4> castling_rights;
		int ep_square;
		int halfmove_clock;
		int fullmove_counter;
};
