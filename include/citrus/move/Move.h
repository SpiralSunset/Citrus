#include <cstdint>
#include <string>

/**
 * \brief Represents a movement of a chess piece.
 * 
 * Here, a piece movement is represented as a 16 bit word.
 * Bits 15-9: where the piece is moving from
 * Bits 8-3: where the piece is moving to
 * Bits 3-0: special move flags
 * 
 * The special move flags indicate what type of move is being performed.
 */
class Move {	
	public:
		/**
		 * \brief The types of moves as represented in the special move flags.
		 */
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
		
		/**
		 * \brief Creates a move from the given information.
		 * 
		 * \param from where the piece is moving from.
		 * \param to where the piece is moving to.
		 * \param move_type the type of move being performed.
		 */
		Move(int from, int to, MoveType move_type);
		
		/// \return where the piece is moving from.
		int from() const;
		
		/// \return where the piece is moving to.
		int to() const;
		
		/// \return the type of move being performed.
		MoveType move_type() const;
		
	private:
		/// \brief The 16 bit word that stores the move.
		uint16_t raw_move;
};
