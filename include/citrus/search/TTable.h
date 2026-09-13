#include "citrus/move/Move.h"
#include <cstdint>
#include <cstddef>
#include <memory>

/**
 * \brief Database of results from searches.
 * 
 * A transposition table is used to quickly compare two positions, disregarding prior moves.
 * If a match is found, the best move from the earlier search can be reused.
 * This is a very effective way to reduce the number of re-searched nodes.
 * 
 * The key (or index) of each table entry is the corresponding position's zobrist hash.
 * There is a chance that two different positions will use the same key (a key collision),
 * but the chances are so low that it generally is not an issue.
 */
class TTable {
	public:
		/**
		 * \brief Contextual information about the score.
		 * 
		 * If the score is cut off by pruning, we cannot use the exact value.
		 * PV-Nodes have exact scores.
		 * Cut-Nodes were cut off by the lower bound.
		 * All-Nodes were cut off by the upper bound.
		 */
		enum class Bound {
			Exact, // PV-Node
			Lower, // Cut-Node
			Upper, // All-Node
		};
		
		/**
		 * \brief Represents a single entry in the table.
		 * 
		 * Each entry must be added by a search before it is considered valid.
		 */
		struct Entry {
			uint64_t key = 0;
			Move best_move;
			int depth = 0;
			int score = 0;
			Bound bound = Bound::Exact;
			bool is_valid = false;
		};
		
		/**
		 * \brief Constructs a transposition table of a given size.
		 * 
		 * Size is heavily recommended to always be 4 mebibytes.
		 * 
		 * \param size the size of the transposition table.
		 */
		explicit TTable(size_t size) : table(std::make_unique<Entry[]>(size)), size(size) {}
		
		/**
		 * \brief Retrieves a reference to a table entry based on a zobrist hash.
		 * 
		 * \param key the zobrist hash.
		 * \return the entry.
		 */
		Entry& operator[](uint64_t key);
	private:
		/**
		 * \brief Pointer to the beginning of the transposition table.
		 * 
		 * Due to the large size of the table, it could not be stored on the stack.
		 * This representation was the solution I came across for storing it in the heap instead.
		 * I truthfully do not know if this is the best way of doing it, but I have yet to run into any issues.
		 */
		std::unique_ptr<Entry[]> table;
		
		/// \brief the size of the transposition table.
		std::size_t size;
};
