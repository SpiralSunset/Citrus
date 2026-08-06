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

std::string Move::to_uci() {
	if (raw_move == 0xFFFF) return "0000";
	std::string str = "";
	str.push_back('a' + (from() % 8));
	str.push_back('1' + (from() / 8));
	str.push_back('a' + (to() % 8));
	str.push_back('1' + (to() / 8));
	if ((uint8_t)move_type() & (uint8_t)MoveType::KnightPromotion) {
		uint8_t promo_compare = (uint8_t)move_type() & ~(uint8_t)MoveType::Capture;
		switch (static_cast<MoveType>(promo_compare)) {
			case MoveType::KnightPromotion:
				str.push_back('n');
				break;
			case MoveType::BishopPromotion:
				str.push_back('b');
				break;
			case MoveType::RookPromotion:
				str.push_back('r');
				break;
			case MoveType::QueenPromotion:
				str.push_back('q');
				break;
		}
	}
	return str;
}
