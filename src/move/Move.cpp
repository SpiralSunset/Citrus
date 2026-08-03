#include "citrus/move/Move.h"
#include <string>

Move::Move() {
	raw_move = 0xFFFF;
}	

Move::Move(int from, int to, MoveType move_type) {
	raw_move = (from << 10) | (to << 4) | (uint8_t)move_type;
}

int Move::from() const {
	return (raw_move >> 10);
}

int Move::to() const {
	return (raw_move >> 4) & 0x3F;
}
		
Move::MoveType Move::move_type() const {
	return static_cast<MoveType>(raw_move & 0xF);
}

std::string Move::to_string() {
	if (raw_move == 0xFFFF) return "invalid";
	std::string str = "";
	str.push_back('a' + (from() % 8));
	str.push_back('1' + (from() / 8));
	str.push_back(' ');
	str.push_back('a' + (to() % 8));
	str.push_back('1' + (to() / 8));
	str.push_back(' ');
	str += std::to_string((uint8_t)move_type());
	return str;
}
