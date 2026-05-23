#include <cstdint>
#include <array>
#include <string>

/**
 * \brief Represents an arrangement of pieces on the game board.
 * 
 * A board contains information about all the pieces in a given position.
 * Specifically, this is the piece color, type, and placement on the grid.
 * 
 * Piece placement is stored in an array of 64 bit words, or "bitboards".
 * Bitboards are an efficient method of placement representation.
 * They function as a set of 64 board squares; either a piece type is there or it is not.
 * Performing set operations on bitboards allow for relatively quick movement and search calculations.
 * 
 * \see https://www.chessprogramming.org/Bitboards
 */
class Board {
	public:
		/**
		 * \brief Represents the color of a piece, or lack thereof.
		 */
		enum Color {
			cWhite,
			cBlack,
			cNone
		};
		
		/**
		 * \brief Represents the type of a piece, or lack thereof.
		 * 
		 * Color is included as a type for set operations (ex: getting all white rooks).
		 */
		enum PieceType {
			White,
			Black,
			Pawn,
			Knight,
			Bishop,
			Rook,
			Queen,
			King,
			None
		};
		
		/**
		 * \brief A combination of a piece type and color.
		 *  
		 * Useful when performing bitboard serialization.
		 */
		struct Piece {
			PieceType pt = None;
			Color c = cNone;
		};
		
		/**
		 * \brief Creates a board with the default starting configuration.
		 */
		Board();
		
		/**
		 * \brief Creates a board with altered piece placement.
		 */
		Board(std::array<uint64_t, 8> new_pieces);
		
		/**
		 * \brief Gets the set of pieces associated with a piece type.
		 * 
		 * \param pt the piece type.
		 * \return the bitboard.
		 */
		uint64_t get_piece_set(PieceType pt) const;
		
		/**
		 * \brief Gets the set of pieces associated with a color.
		 * 
		 * \param c the color.
		 * \return the bitboard.
		 */
		uint64_t get_piece_set(Color c) const;
		
		/**
		 * \brief Gets the set of pieces associated with a piece type and color.
		 * 
		 * \param pt the piece type.
		 * \param c the color.
		 * \return the bitboard.
		 */
		uint64_t get_piece_set(PieceType pt, Color c) const;
		
		/**
		 * \brief Serializes the board into a list of pieces on squares.
		 * 
		 * \return serialized board.
		 * \todo Rewrite with a more efficient implementation.
		 */
		std::array<Piece, 64> to_array();
		
	private:
		/// \brief The bitboards of each piece type.
		std::array<uint64_t, 8> pieces;
};
