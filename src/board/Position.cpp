#include "citrus/board/Position.h"
#include <algorithm>

Position::Position() {
	side_to_move = Board::Color::cWhite;
	castling_rights.fill(true);
	ep_square = -1;
	halfmove_clock = 0;
	fullmove_counter = 0;
}

Position::Position(Board new_board, Board::Color new_side_to_move, std::array<bool, 4> new_castling_rights, int new_ep_square, int new_halfmove_clock, int new_fullmove_counter) {
	board = new_board;
	side_to_move = new_side_to_move;
	std::copy(new_castling_rights.begin(), new_castling_rights.end(), castling_rights.begin());
	ep_square = new_ep_square;
	halfmove_clock = new_halfmove_clock;
	fullmove_counter = new_fullmove_counter;
}


Board Position::get_board() const {
	return board;
}

Board::Color Position::get_side_to_move() const {
	return side_to_move;
}

bool Position::get_castling_right(Position::CastlingRight cr) const {
	return castling_rights[(uint8_t)cr];
}

int Position::get_ep_square() const {
	return ep_square;
}

int Position::get_halfmove_clock() const {
	return halfmove_clock;
}

int Position::get_fullmove_counter() const {
	return fullmove_counter;
}
