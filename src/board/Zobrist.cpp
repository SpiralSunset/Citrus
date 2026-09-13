#include "citrus/board/Zobrist.h"

Zobrist::Zobrist() {
	std::mt19937_64 rng(SEED);
	
	for (int i = 0; i < NUMBERS_SIZE; i++) {
		numbers[i] = rng();
	}
}

uint64_t Zobrist::get_piece_hash(Board::PieceType pt, Board::Color c, int sq) {
	int piece_idx = pt - 2; // pt 0 and 1 are colors
	int color_idx = (c == Board::Color::cBlack) ? 1 : 0;
	int idx = (color_idx * 6 + piece_idx) * 64 + sq;
	return numbers[idx];
}

uint64_t Zobrist::get_side_hash(Board::Color c) {
	if (c == Board::Color::cWhite) return 0;
	return numbers[NUMBERS_SIDE_TO_MOVE];
}

uint64_t Zobrist::get_castle_hash(int cr_idx) {
	return numbers[NUMBERS_CASTLING + cr_idx];
}

uint64_t Zobrist::get_ep_hash(int sq) {
	return numbers[NUMBERS_EP + (sq % 8)];
}
