#include "citrus/board/Board.h"
#include <random>

/**
 * \brief Handles the generation of zobrist hashes.
 * 
 * Zobrist hashes are (almost) unique numbers that map to any chess position.
 * Two similar positions will generate different hash codes.
 * This allows positions to be compared quickly and stored efficiently.
 * 
 * These hash codes are primarily used within the transposition table.
 * 
 * \see https://chessprogramming.org/Zobrist_Hashing
 */
class Zobrist {
	public:
		/// \brief Index of side to move hash.
		constexpr static int NUMBERS_SIDE_TO_MOVE = 12*64;
		
		/// \brief Index of first castling hash.
		constexpr static int NUMBERS_CASTLING = NUMBERS_SIDE_TO_MOVE+1;
		
		/// \brief Index of the first en passant column hash.
		constexpr static int NUMBERS_EP = NUMBERS_CASTLING+4;
		
		/// \brief Size of the stored hashes.
		constexpr static int NUMBERS_SIZE = NUMBERS_EP+8; // 781
		
		/**
		 * \brief Initializes every hash with pseudorandom 64bit numbers.
		 * 
		 * Hashes may not be the same on differing hardware.
		 */
		Zobrist();
		
		/** 
		 * \brief Gets the hash of a specific piece on a square.
		 * 
		 * \param pt the piece type.
		 * \param c the color.
		 * \param sq the square index.
		 * \return the hash.
		 */
		uint64_t get_piece_hash(Board::PieceType pt, Board::Color c, int sq);
		
		/** 
		 * \brief Gets the hash of a side to move.
		 * 
		 * If the side to move is white, it will always return 0.
		 * 
		 * \param c the color.
		 * \return the hash.
		 */
		uint64_t get_side_hash(Board::Color c);
		
		/** 
		 * \brief Gets the hash of a castling right.
		 * 
		 * This follows the numbering of Position::CastlingRight.
		 * 
		 * \param cr_idx the index of the castling right (0-4).
		 * \return the hash.
		 */
		uint64_t get_castle_hash(int cr_idx);
		
		/** 
		 * \brief Gets the hash of a en passant column.
		 * 
		 * When unhashing, we can calculate the square because we know the side to move and ep column.
		 * 
		 * \param sq the square index.
		 * \return the hash.
		 */
		uint64_t get_ep_hash(int sq);
	
	private:		
		/// \brief Seed for the random number generator.
		constexpr static uint64_t SEED = 0x0A56A5F0BFE39272;
		
		/// \brief Internal list of hashes. Split into 4 distinct sections for each hash type.
		std::array<uint64_t, NUMBERS_SIZE> numbers;
};
