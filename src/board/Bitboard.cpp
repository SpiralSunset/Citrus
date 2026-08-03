#include "citrus/board/Bitboard.h"
#include <cstdint>

int popcount(uint64_t bb) {
	return __builtin_popcountll(bb);
}

int pop_lsb(uint64_t &bb) {
	int idx  = __builtin_ctzll(bb);
	bb &= bb - 1;
	return idx;
}
