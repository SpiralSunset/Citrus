#pragma once

#include "citrus/move/Move.h"
#include <array>

/**
 * \brief List of Moves with variable size.
 * 
 * Preferable performance compared to a dynamically allocated equivalent.
 */
class MoveList {
	public:
		/**
		 * \brief Appends a Move to the end of the list.
		 * 
		 * \param move the Move to be added.
		 */
		void add(Move move);
		
		/**
		 * \brief Sets the size of the list to zero, which functionally clears its contents.
		 */
		void clear();
		
		/**
		 * \brief Gets the size of the list.
		 * 
		 * \param the size of the list.
		 */
		int get_size();
		
		/**
		 * \brief Retrieves a Move from the corresponding list index.
		 * 
		 * \param index the list index.
		 * \return reference to the appropriate Move.
		 */
		Move& operator[](int index);
	private:
		/// \brief Maximum size of the move list.
		static constexpr int MAX_MOVES = 256;
		
		/// \brief Array of moves.
		std::array<Move, MAX_MOVES> moves;
		
		/// \brief Current size of the move list.
		int size = 0;
};
