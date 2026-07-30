#include "citrus/move/MoveList.h"

void MoveList::add(Move move) {
	moves[size++] = move;
}

void MoveList::clear() {
	size = 0;
}

int MoveList::get_size() {
	return size;
}

Move& MoveList::operator[](int index) {
	return moves[index];
}
