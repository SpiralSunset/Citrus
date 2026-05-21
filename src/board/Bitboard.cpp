#include "citrus/board/Bitboard.h"
#include <cstdint>

int pop_count(uint64_t bb) {
	return __builtin_popcount(bb);
}

int pop_lsb(uint64_t &bb) {
	int idx  = __builtin_ctzll(bb);
	bb &= bb - 1;
	return idx;
}
