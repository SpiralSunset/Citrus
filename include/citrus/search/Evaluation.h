#include "citrus/board/Position.h"

/**
 * \brief Utilities for determining the value of a position.
 * 
 * These are used to provide position scores for the Searcher.
 * The score is measured in centipawns, or one-hundredth of a pawn.
 */
namespace Evaluation {
	/**
	 * \brief Retrieves the value of a piece type.
	 * 
	 * \param pt the piece type.
	 * \return the value in centipawns.
	 */
	constexpr int piece_value(Board::PieceType pt);
	
	/**
	 * \brief Retrieves the phase weight of a piece type.
	 * 
	 * The phase is a number between 0 and 24 that represents the game phase.
	 * 
	 * \param pt the piece type.
	 * \return the phase weight.
	 */
	constexpr int phase_weight(Board::PieceType pt);
	
	/// \brief The phase value when no pieces have been captured yet.
	constexpr int TOTAL_PHASE = 32;
	
	/**
	 * \brief Evaluates the value of a given position.
	 * 
	 * If a position favors the white side, the number will be more positive.
	 * Likewise, a position favoring black will be more negative.
	 * 
	 * The following are used to determine the output:
	 * - Material
	 * - Piece Square Tables
	 * - Tapered Eval
	 */
	int evaluate(Position &pos);
}
