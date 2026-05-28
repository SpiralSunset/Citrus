#include "citrus/board/Board.h"
#include "citrus/move/Move.h"

/**
 * \brief Represents the game state on a particular turn.
 * 
 * A chess position is an arrangement of pieces, whose turn it is, what castling is permitted,
 * the target square for en passant (or lack thereof), the Halfmove Clock, and the number
 * of black turns that have occurred since the game's beginning.
 * 
 * Essentially, it is the game at a specific moment.
 */
class Position {
	public:
		/**
		 * \brief Represents the types of conditions required to castle.
		 */
		enum class CastlingRight : uint8_t {
			wKing = 0,
			wQueen = 1,
			bKing = 2,
			bQueen = 3
		};
	
		/**
		 * \brief Creates the default position at the very start of the game.
		 */
		Position();
		
		/**
		 * \brief Creates a position with given parameters.
		 * 
		 * \param new_board the board.
		 * \param new_side_to_move the side to move.
		 * \param new_castling_rights list of castling rights.
		 * \param new_ep_square the en passant target square.
		 * \param new_halfmove_clock the updated halfmove clock.
		 * \param new_fullmove_counter the update fullmove counter.
		 */
		Position(Board new_board, Board::Color new_side_to_move = Board::Color::cWhite, std::array<bool, 4> new_castling_rights = {true, true, true, true}, int new_ep_square = -1, int new_halfmove_clock = 0, int new_fullmove_counter = 0);
		
		/// \return the board containing the piece placement.
		Board get_board() const;
		
		/// \return the side whose turn it is to move.
		Board::Color get_side_to_move() const;
		
		/**
		 * \brief Retrieves whether a condition that allows castling is true.
		 * 
		 * \param cr the castling right.
		 * \return whether the condition allows castling.
		 */
		bool get_castling_right(CastlingRight cr) const;
		
		/// \return the en passant target square.
		int get_ep_square() const;
		
		/// \return the halfmove clock for the fifty-move rule.
		int get_halfmove_clock() const;
		
		/// \return the fullmove counter.
		int get_fullmove_counter() const;
		
		/**
		 * \brief Updates the position with a given move.
		 * 
		 * This involves changing every aspect of the position.
		 * For example, it will update the piece placement and swap which color is moving.
		 * In a search tree, this function converts a parent node to a child node.
		 * 
		 * \param move the move being made.
		 */
		void make_move(Move move);

	private:
		/// \brief The piece placement of this position.
		Board board;
		
		/// \brief Which side will be moving this turn.
		Board::Color side_to_move;
		
		/// \brief List of castling rights.
		std::array<bool, 4> castling_rights;
		
		/// \brief The en passant target square (from 0-63) or -1 (if there is none).
		int ep_square;
		
		/// \brief Number of halfmoves since the last pawn move or capture.
		int halfmove_clock;
		
		/// \brief Fullmove number, starting at 1 and incremented after each Black move.
		int fullmove_counter;
};
