#pragma once

#include "citrus/move/MoveGenerator.h"

/**
 * \brief Performance test utility.
 * 
 * Perft is a debugging tool that counts the total number of nodes reachable from a given position.
 * This number can then be compared with existing chess engines to verify the integrity of the move maker and generator.
 * 
 * In the future, there will be additional functionality to test the computation speed of movement optimizations. 
 */
namespace Perft {
	/// \brief The maximum ply depth a test can evaluate to.
	constexpr int MAX_SEARCH_PLY = 128;
	
	/**
	 * \brief Runs perft on a given position.
	 * 
	 * Typically, that position will be the board's default starting configuration.
	 * 
	 * \param pos reference to the position.
	 * \param mg reference to the move generator.
	 * \param depth ply depth of the tree.
	 * \return final node count.
	 */
	uint64_t run(Position &pos, MoveGenerator &mg, int depth);
	
	/**
	 * \brief Recursive perft with an initialized list stack.
	 * 
	 * The performance improvement of using a stack of MoveLists here, rather than simply copying arrays of moves, is negligible.
	 * However, this method will become more useful when optimizing the search algorithm.
	 * 
	 * \param pos reference to the position.
	 * \param mg reference to the move generator.
	 * \param list_stack reference to the pre-initialized stack of MoveLists.
	 * \param depth ply depth of the tree.
	 * \return final node count.
	 */
	uint64_t run(Position &pos, MoveGenerator &mg, std::array<MoveList, MAX_SEARCH_PLY> &list_stack, int depth);
	
	/**
	 * \brief Runs perft on every legal move of a position.
	 * 
	 * The results of each move will be printed via iostream.
	 * 
	 * \param pos reference to the position.
	 * \param mg reference to the move generator.
	 * \param depth ply depth of the tree.
	 */
	void divide(Position &pos, MoveGenerator &mg, int depth);

	/**
	 * \brief Coverts the square index of a board to the UCI equivalent.
	 * 
	 * For example, 0 becomes a1, 7 becomes h1, and 63 becomes h8.
	 * 
	 * This function will be moved to a more appropriate parent as soon as it is possible to do so.
	 * 
	 * \param sq the square index.
	 * \return the equivalent string.
	 * 
	 * \todo Move function to appropriate parent.
	 */
	std::string square_to_uci(int sq);
};
