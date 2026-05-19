#include <cstdint>
#include <string>

/*
A move is a 16 bit word.
Bits 15-9: where the piece is moving from
Bits 8-3: where the piece is moving to
Bits 3-0: special move flags
*/
class Move {	
	public:
		enum class MoveType : std::uint8_t {
			Quiet = 0,
			DoublePawnPush = 1,
			KingCastle = 2,
			QueenCastle = 3,
			Capture = 4,
			EpCapture = 5,

			KnightPromotion = 8,
			BishopPromotion = 9,
			RookPromotion = 10,
			QueenPromotion = 11,

			KnightPromotionCapture = 12,
			BishopPromotionCapture = 13,
			RookPromotionCapture = 14,
			QueenPromotionCapture = 15
		};
		
		Move(int from, int to, MoveType move_type);
		
		int from() const;
		int to() const;
		MoveType move_type() const;
		
		std::string to_string() const;
		
	private:
		uint16_t raw_move;
};
