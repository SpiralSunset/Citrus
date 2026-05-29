#include "citrus/move/Move.h"

Move::Move() {
	raw_move = 0xFF;
}	

Move::Move(int from, int to, MoveType move_type) {
	raw_move = (from << 10) | (to << 4) | (uint8_t)move_type;
}

int Move::from() const {
	return (raw_move >> 10);
}

int Move::to() const {
	return (raw_move >> 4) & 0x3f;
}
		
Move::MoveType Move::move_type() const {
	return static_cast<MoveType>(raw_move & 0xF);
}
