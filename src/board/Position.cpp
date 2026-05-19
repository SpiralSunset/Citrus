#include "citrus/board/Position.h"

Position::Position() {
	side_to_move = Board::Color::cWhite;
	castling_rights[Board::Color::cWhite] = true;
	castling_rights[Board::Color::cBlack] = true;
	ep_square = -1;
}

Position::Position(Board new_board, Board::Color new_side_to_move, bool new_castling_rights[2], int new_ep_square) {
	board = new_board;
	side_to_move = new_side_to_move;
	castling_rights[0] = new_castling_rights[0];
	castling_rights[1] = new_castling_rights[1];
	ep_square = new_ep_square;
}


Board Position::get_board() const {
	return board;
}

Board::Color Position::get_side_to_move() const {
	return side_to_move;
}

bool Position::get_castling_rights(Board::Color c) const {
	return castling_rights[c];
}

int Position::get_ep_square() const {
	return ep_square;
}
