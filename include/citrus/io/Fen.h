#include "citrus/board/Position.h"
#include <string>

/**
 * \brief Converts between FEN strings and game positions.
 * 
 * Forsyth-Edwards Notation (FEN) is a one-line ASCII string representing a chess position.
 * 
 * \see https://www.chessprogramming.org/Forsyth-Edwards_Notation
 * 
 * It is implemented here to expedite the testing process, as the format is much more
 * informationally concise than a grid printout or the raw data.
 * 
 * In the future, it will be mostly replaced by a proper protocol interface (likely UCI).
 */
class Fen {
	public:
		/**
		 * \brief Converts a position into a FEN string.
		 * 
		 * \param pos the position to be converted.
		 * \return the output FEN string.
		 */
		std::string pos_to_str(Position pos);
		
		/**
		 * \brief Converts a FEN string into a position.
		 * 
		 * \param str the string to be converted.
		 * \return the output board position.
		 * 
		 * \todo Add formatting verification so that invalid strings are rejected.
		 */
		Position str_to_pos(std::string str);
	private:
		/**
		 * \brief Gives a char representing a piece in FEN.
		 * 
		 * White pieces are capitalized, while black pieces are lower case.
		 * 
		 * \param p the piece to be converted.
		 * \return the char representing the piece.
		 */
		char piece_to_char(Board::Piece p);
		
		/**
		 * \brief Gives a piece type based on a FEN char.
		 * 
		 * \param c the char to be converted.
		 * \return the piece type associated with the char.
		 */
		Board::PieceType char_to_piece_type(char c);
};
