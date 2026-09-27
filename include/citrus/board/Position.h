#pragma once

#include "citrus/board/Board.h"
#include "citrus/move/Move.h"
#include "citrus/board/Zobrist.h"
#include <stack>

/**
 * \brief Represents the game state on a particular turn.
 * 
 * A chess position is an arrangement of pieces, whose turn it is, what castling is permitted,
 * the target square for en passant (or lack thereof), the Halfmove Clock, and the number
 * of black turns that have occurred since the game's beginning.
 * 
 * Essentially, it is the game at a specific moment.
 * 
 * \todo Add function for retrieving occupancy
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
		 * \brief Information needed to unmake a move.
		 * 
		 * It is added and removed from the history with make_move() and unmake_move() respectively.
		 */
		struct UndoInfo {
			Board::Piece cap_target;
			std::array<bool, 4> castling_rights;
			int ep_square;
			int halfmove_clock;
			uint64_t hash;
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
		const Board& get_board() const;
		
		/// \return the side whose turn it is to move.
		Board::Color get_side_to_move() const;
		
		/// \return the side who will be moving after the current turn is complete.
		Board::Color get_next_to_move() const;
		
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
		
		/// \return the zobrist hash.
		uint64_t get_zobrist_key() const;
		
		/**
		 * \brief Creates the zobrist hash for the current position.
		 * 
		 * For more information, see Zobrist.
		 */
		void create_hash();

		/**
		 * \brief Checks if the current position has a threefold repetition.
		 *
		 * A threefold repetition happens when the exact same board position occurs three times in a game.
		 * Under most chess rulesets, this allows either player to claim a stalemate.
		 */
		bool has_threefold_repetition();
		
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
		
		/**
		 * \brief Reverts the position to how it was before the last move.
		 * 
		 * This is the inverse of make_move().
		 * Elements of a position that cannot be reversed are restored from the history.
		 * In a search tree, this function reverts a child node to its parent node.
		 * 
		 * \param move the move being unmade.
		 */
		void unmake_move(Move move);

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
		
		/// \brief Current zobrist hash.
		uint64_t hash;
		
		/// \brief Responsible for generating the zobrist hash.
		Zobrist zobrist;
		
		/// \brief Movement history to be restored with unmake_move().
		std::vector<UndoInfo> history;
		
		/**
		 * \brief Disables a castling right on both board representations.
		 * 
		 * \param cr the castling right in question.
		 */
		void disable_castling_right(CastlingRight cr);
		
		/**
		 * \brief Adds a piece to both board representations.
		 * 
		 * \param pt the piece type.
		 * \param c the color.
		 * \param sq the square index.
		 */
		void add_piece(Board::PieceType pt, Board::Color c, int sq);
		
		/**
		 * \brief Removes a piece from both board representations.
		 * 
		 * \param pt the piece type.
		 * \param c the color.
		 * \param sq the square index.
		 */
		void remove_piece(Board::PieceType pt, Board::Color c, int sq);
		
		/**
		 * \brief Updates a piece on both board representations.
		 * 
		 * This is slightly faster than calling add_piece() and remove_piece() consecutively, so it should be used where possible.
		 * 
		 * \param pt the piece type.
		 * \param c the color.
		 * \param sq the square index.
		 * \param capture whether the moving piece is capturing another piece.
		 */
		void update(Board::PieceType pt, Board::Color c, int from, int to, bool capture = false);
};
