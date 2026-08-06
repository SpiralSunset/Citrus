#include "citrus/search/Searcher.h"
#include <iostream>
#include <string>
#include <sstream>
#include <atomic>
#include <thread>

/**
 * \brief Handles communciation with user interfaces.
 * 
 * Universal Chess Interface, or UCI, is a popular option for allowing compatibility
 * with GUIs, and by extension, other chess engines.
 * 
 * \see https://en.wikipedia.org/wiki/Universal_Chess_Interface
 * 
 * \todo Support more commands, options, and features.
 */
class UCI {
	public:
		/// \brief Determines whether the search is set to stop.
		std::atomic<bool> stop_search;
	
		/// \brief Responsible for carrying out "go" commands.
		Searcher searcher;
		
		/// \brief Root position to be searched from.
		Position position;
		
		/// \brief Result of the latest best move search.
		Move best_move;
		
		/// \brief Thread for running searches.
		std::thread search_thread;
	
		/// \brief Intializes stop_search and searcher.
		UCI();
	
		/**
		 * \brief Converts a move string in UCI format to a Move.
		 * 
		 * Since UCI moves leave out certain information, a position is required
		 * context for determining the move type.
		 * 
		 * \param move_str the string to be converted.
		 * \return the corresponding move.
		 */
		Move parse_move(std::string move_str);
		
		/**
		 * \brief Sets the game position in accordance with the position command.
		 * 
		 * \param iss reference to the istringstream holding the remaining text of the latest command.
		 */
		void handle_position(std::istringstream &iss);
		
		/**
		 * \brief Searches for moves in accordance with the go command.
		 * 
		 * \param iss reference to the istringstream holding the remaining text of the latest command.
		 */
		void handle_go(std::istringstream &iss);
		
		/**
		 * \brief Starts UCI mode with standard io.
		 * 
		 * Should be started by running "./Citrus UCI"
		 */
		void run();
};
