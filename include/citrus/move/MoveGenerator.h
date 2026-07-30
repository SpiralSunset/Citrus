#pragma once

#include "citrus/board/Position.h"
#include "citrus/move/MoveList.h"
#include <cstdint>
#include <array>

/**
 * \brief Responsible for the generation of all legal and pseudo-legal moves.
 * 
 * The main function of a MoveGenerator is to calculate all possible moves that can be made from a given Position.
 * Then, it will pass these moves to the search system, which will select a Move to be played.
 * 
 * There are two kinds of move generation.
 * Legal moves exclude options that would leave their king in check within the resulting Position.
 * Pseudo-legal moves do not do this, and are consequently faster to calculate.
 * 
 * \todo Extensive Perft testing to ensure flawless generation.
 */
class MoveGenerator {
	public:
		/**
		 * \brief Creates a move generator and populates the movement tables for non-sliding pieces.
		 */
		MoveGenerator();
		
		/**
		 * \brief Generates all possible pseudo-legal moves from a given position.
		 * 
		 * \param pos the position to be searched.
		 * \param moves the list of moves to be written to.
		 */
		void gen_pseudo_legal_moves(Position &pos, MoveList &moves);
		
		/**
		 * \brief Evaluates whether the opposing king is currently in check.
		 * 
		 * This is done from the perspective of the player whose turn it is to move.
		 * For example, if the current side to move is white, the black king's status will be returned.
		 * 
		 * It is primarily used to check if a pseudo-legal move is legal.
		 * 
		 * \param pos the position to be searched.
		 * \param last_move_type the last type of move made.
		 * \return whether the opposing king is in check.
		 */
		bool is_opposing_king_in_check(Position &pos, Move::MoveType last_move_type);

	private:
		/// \brief All possible knight moves indexed by square.
		std::array<uint64_t, 64> knight_moves;
		
		/// \brief All possible king moves indexed by square.
		std::array<uint64_t, 64> king_moves;
		
		/// \brief All possible pawn attacks indexed by color and square.
		std::array<std::array<uint64_t, 64>, 2> pawn_attacks;
		
		/**
		 * \brief A simple raycast-based method of sliding piece attack generation.
		 * 
		 * It is somewhat slower than more advanced approaches and is not intended to be the permanent primary implementation.
		 * 
		 * \param sq the square index of the sliding piece.
		 * \param occ the bitboard representing all occupied spaces.
		 * \param df the incremental change in file.
		 * \param dr the incremental change in rank.
		 * \return a bitboard containing the spaces accessible to the piece.
		 */
		uint64_t slow_ray_attacks(int sq, uint64_t occ, int df, int dr);
		
		/**
		 * \brief Calculates a bitboard containing a bishop's possible movement.
		 * 
		 * \param sq the square index of the bishop.
		 * \param the bitboard representing all occupied spaces.
		 * \return a bitboard containing the spaces accessible to the bishop.
		 * 
		 * \todo Replace raycast generation with magic bitboards.
		 */
		uint64_t get_bishop_attacks(int sq, uint64_t occ);
		
		/**
		 * \brief Calculates a bitboard containing a rook's possible movement.
		 * 
		 * \param sq the square index of the rook.
		 * \param the bitboard representing all occupied spaces.
		 * \return a bitboard containing the spaces accessible to the rook.
		 * 
		 * \todo Replace raycast generation with magic bitboards.
		 */
		uint64_t get_rook_attacks(int sq, uint64_t occ);
		
		/**
		 * \brief Appends moves of a specific type to a list of moves.
		 * 
		 * \param moves the list of moves to be written to.
		 * \param sq the square index of the piece being moved.
		 * \param targets the bitboard containing every location the moved piece can go to.
		 * \param move_type the type of move to be made.
		 */
		void append_moves(MoveList &moves, int sq, uint64_t targets, Move::MoveType move_type);
};
