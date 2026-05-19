#include "citrus/board/Board.h"
#include <algorithm>

Board::Board() {
	pieces = {
		0x000000000000FFFF, // White
		0xFFFF000000000000, // Black
		0x00FF00000000FF00, // Pawns
		0x4200000000000042, // Knights
		0x2400000000000024, // Bishops
		0x8100000000000081, // Rooks
		0x0800000000000008, // Queens
		0x8000000000000080  // Kings
	};
}

Board::Board(std::array<uint64_t, 8> new_pieces) {
	std::copy(new_pieces.begin(), new_pieces.end(), pieces.begin());
}

uint64_t Board::get_piece_set(PieceType pt) const {
	return pieces[pt];
}

uint64_t Board::get_piece_set(Color c) const {
	return pieces[c];
}

uint64_t Board::get_piece_set(PieceType pt, Color c) const {
	return pieces[pt] & pieces[c];
}
