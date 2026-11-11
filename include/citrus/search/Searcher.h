#include "citrus/move/MoveGenerator.h"
#include "citrus/search/TTable.h"
#include <chrono>
#include <atomic>

/**
 * \brief The system responsible for finding good moves to play.
 * 
 * This is the "brain" of the program; the core of the decision-making process.
 * It looks ahead at the different paths a game can take and determines the
 * one with the highest chance of winning.
 */
class Searcher {
	public:
		/**
		 * \brief The information given by a search.
		 * 
		 * The score and chosen move are needed to find the best move.
		 * The rest are there to provide information about the search node.
		 */
		struct SearchResult {
			int score;
			Move chosen_move;
			uint64_t node;
		};
		
		/**
		 * \brief Options that determine the type of search to be conducted.
		 * 
		 * All time is measured in milliseconds.
		 * 
		 * - depth: local maximum search depth
		 * - wtime / btime: time remaining for either side
		 * - winc / binc: time taken to make latest move
		 * - movestogo: halfmoves until next time control
		 * - max_nodes: maximum nodes to be searched
		 * - movetime: maximum time to search
		 * - finite_nodes: indicates that search must be stopped once max_nodes is reached
		 * - finite_time: indicates that search must be stopped once movetime is reached
		 * 
		 * Several of these currently do nothing.
		 */
		struct SearchSettings {
			int depth = 7;
			int wtime = 0;
			int btime = 0;
			int winc = 0;
			int binc = 0;
			int movestogo = 0;
			int max_nodes = 0;
			int movetime = 0;
			bool finite_nodes = false;
			bool finite_time = false;
		};
		
		/// \brief The maximum search depth.
		constexpr static int MAX_SEARCH_PLY = 128;
		
		/// \brief Local settings for current search.
		SearchSettings settings;
		
		/// \brief Reference to search stopping flag.
		std::atomic<bool> &stopping;
		
		/// \brief Point in time where the search started.
		std::chrono::steady_clock::time_point start_time;
		
		/**
		 * \brief Constructs a searcher.
		 * 
		 * \param stopper reference to the search stopping flag.
		 */
		Searcher(std::atomic<bool> &stopper) : stopping(stopper), ttable(1 << 22) {}
		
		/**
		 * \brief Determines whether a evaluation score indicates checkmate.
		 * 
		 * \param score the score to be examined.
		 * \return whether the score indicates checkmate.
		 */
		bool is_mate_score(int score);
		
		/**
		 * \brief Determines whether the movetime has been reached.		 *
		 * \todo Base case must account for checkmate and stalemate.
		 * 
		 * Getting the current time is not computationally free, so this should not be run every node.
		 * 
		 * \return whether movetime has been reached.
		 */
		bool time_limit_reached();
		
		/**
		 * \brief Ranks a move's priority to be picked.
		 * 
		 * From highest to lowest, the move priority is:
		 * - Ply move from previous iteration (not handled here)
		 * - Best move from the transposition table
		 * - Is a capture
		 * - Quiet move that previously caused a beta cutoff
		 * - Everything else
		 * 
		 * More complexity will be added over time.
		 * 
		 * \param move the move to be scored.
		 * \return the score.
		 */
		int move_order_score(Position &pos, Move move, int ply, Move tt_best);
		
		/**
		 * \brief Picks a move from a MoveList with the highest chance of being good.
		 * 
		 * This is responsible for the move ordering of the search, and is what makes iterative deepening effective.
		 * By picking the likely best move first, a significant number of nodes can be pruned.
		 * 
		 * It functions like a selection search, but usually does not sort every move.
		 * The source of move_idx must be incremented after every use for it to work as intended.
		 * 
		 * \param move_list reference to the move list.
		 * \param move_idx index of the start of the available moves.
		 * \param ply current ply of the search, to compare to the pv table.
		 * \return the highest priority move from the list.
		 */
		Move pick_move(Position &pos, MoveList &move_list, int move_idx, int ply, Move tt_best);
		
		/**
		 * \brief Finds the ideal move that can be made from a given position.
		 * 
		 * It is a depth-first negamax search. The larger the depth, the more informed
		 * the search will be. However, this will also exponentially increase the time it
		 * takes to arrive at an answer.
		 * 
		 * The following optimizations are in use:
		 * - Alpha-beta pruning
		 * - Iterative deepening
		 * - Move ordering
		 * - Transposition table
		 * 
		 * \param pos the current position of the game.
		 * \return the best move that can be made.
		 */
		Move find_best_move(Position pos);
		
	private:
		/// \brief Arbitrarily high integer.
		constexpr static int INF = 9999999;
		
		/// \brief Value of a position in checkmate.
		constexpr static int MATE = 5000000;

		/// \brief Max value that the history heuristic can have.
		constexpr static int MAX_HISTORY = 16384;
		
		/// \brief Pre-allocated MoveLists indexed by depth.
		std::array<MoveList, MAX_SEARCH_PLY> list_stack;
		
		/// \brief Generates moves for the search.
		MoveGenerator move_generator;
		
		/// \brief The number of nodes generated by the current/last search.
		uint64_t node_count = 0;
		
		/// \brief Triangular principal variation table.
		Move pv_table[MAX_SEARCH_PLY][MAX_SEARCH_PLY];
		
		/// \brief Lengths of search principal variation in pv_table.
		int pv_length[MAX_SEARCH_PLY];
		
		/// \brief Root principal variation from the last iteration.
		Move previous_pv[MAX_SEARCH_PLY];
		
		/// \brief Length of previous_pv.
		int previous_pv_length;
		
		/// \brief The transposition table.
		TTable ttable;

		/**
		 * \brief Table of values used for the MVV-LVA heuristic in the move ordering.
		 *
		 * MVV-LVA is used to quickly compare the value of captures. 
		 * For example, a pawn capturing a rook is likely to be more worthwhile than a pawn capturing another pawn.
		 *
		 * The rows represent the captured piece, and the columns represent the capturing piece.
		 * They are arranged in the order described for Board::PieceType.
		 */
		int mvv_lva[6][6] {
			{10, 13, 13, 15, 18, 0},
			{30, 33, 33, 35, 38, 0},
			{30, 33, 33, 35, 38, 0},
			{50, 53, 53, 55, 58, 0},
			{80, 53, 53, 55, 58, 0},
			{ 0,  0,  0,  0,  0, 0}
		};

		/// \brief The table of killer moves.
		Move killer_table[MAX_SEARCH_PLY][2];

		/// \brief The history table.
		int history_table[2][64][64] = {};

		/// \brief The current list of quiet moves.
		MoveList quiet_list;

		/**
		 * \brief Adds a killer move to the killer table.
		 * 
		 * \param move The move to be added.
		 * \param ply The ply at which that move occurred.
		 */
		void add_killer(Move move, int ply);

		/**
		 * \brief Updates the history table with a move.
		 *
		 * \param side_to_move The current side to move.
		 * \param move The move to update.
		 * \param bonus The calculated bonus to be considered.
		 */
		void update_history(Board::Color side_to_move, Move move, int bonus);

		/**
		 * \brief The quiescence search performed at the end of the negamax leaf nodes.
		 *
		 * When choosing a move in the negamax search, stopping immediately at depth 0 could be problematic.
		 * There is a chance that a unevaluated, detrimental move could happen immediately after.
		 * We want the search to end in a position where there are no important tactical decisions to be made.
		 * The purpose of quiescence search is to do exactly that.
		 * 
		 * \param pos the position that moves will be derived from
		 * \param alpha minimum assured score of maximizing side
		 * \param beta maximum assured score of minimizing side
		 * \param ply the current ply of the search
		 * \return the best move (and related info) from the position.
		 */
		SearchResult quiesce(Position &pos, int alpha, int beta, int ply);
			
		/**
		 * \brief Recursive search to find the highest-value move from a position.
		 * 
		 * Results are dependent on the current search settings.
		 * 
		 * \param pos the position that moves will be derived from.
		 * \param depth the current depth of the search
		 * \param alpha minimum assured score of maximizing side
		 * \param beta maximum assured score of minimizing side
		 * \param ply the current ply of the search
		 * \return the best move (and related info) from the position.
		 */
		SearchResult negamax(Position &pos, int depth, int ply, int alpha, int beta);
};
