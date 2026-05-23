#include "citrus/board/Board.h"
#include "citrus/board/Bitboard.h"
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
		0x1000000000000010  // Kings
	};
}

Board::Board(std::array<uint64_t, 8> new_pieces) {
	std::copy(new_pieces.begin(), new_pieces.end(), pieces.begin());
}

uint64_t Board::get_piece_set(PieceType pt) const {
	if (pt == Board::PieceType::None) return 0;
	return pieces[pt];
}

uint64_t Board::get_piece_set(Color c) const {
	if (c == Board::Color::cNone) return 0;
	return pieces[c];
}

uint64_t Board::get_piece_set(PieceType pt, Color c) const {
	if (pt == Board::PieceType::None || c == Board::Color::cNone) return 0;
	return pieces[pt] & pieces[c];
}

std::array<Board::Piece, 64> Board::to_array() {
	std::array<Board::Piece, 64> out;
	
	for (int i = Board::Color::cWhite; i <= Board::Color::cBlack; ++i) {
		for (int j = Board::PieceType::Pawn; j <= Board::PieceType::King; ++j) {
			Board::Piece p;
			p.pt = static_cast<Board::PieceType>(j);
			p.c = static_cast<Board::Color>(i);
			
			uint64_t bb = Board::get_piece_set(p.pt, p.c);
			while (bb) {
				int idx = pop_lsb(bb);
				out[idx] = p;
			}
		} 
	}
	
	return out;
}
