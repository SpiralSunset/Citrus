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
 * 
 * One weakness of a pure bitboard representation is that it is not optimized for direct
 * piece lookup by square. For this reason, an auxiliary square-centric board representation is
 * maintained as well.
 * 
 * \todo Replace enums with strongly typed enum classes.
 * \todo Static default piece positions, to be used in castling-related calculations.
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
		 * Primarily used to represent a piece in the square-centric representation.
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
		 * \brief Retrieves a single piece from a square index (0-63).
		 * 
		 * \param sq the square index.
		 * \return the piece.
		 */
		Piece get_square(int sq);
		
		/**
		 * \brief Adds a piece to the board.
		 * 
		 * Using update() is more efficient than using this and remove_piece() together.
		 * This function is primarily used for implementing special move types.
		 * 
		 * \param pt the piece type.
		 * \param c the color.
		 * \param sq the square index.
		 */
		void add_piece(PieceType pt, Color c, int sq);
		
		/**
		 * \brief Removes a piece from the board.
		 * 
		 * Using update() is more efficient than using this and add_piece() together.
		 * This function is primarily used for implementing special move types.
		 * 
		 * \param pt the piece type.
		 * \param c the color.
		 * \param sq the square index.
		 */
		void remove_piece(PieceType pt, Color c, int sq);
		
		/**
		 * \brief Updates a piece's position on the board.
		 * 
		 * This is the default way of handling piece movement.
		 * The piece is removed from its previous position and added to a new one.
		 * If performing a capture, the captured piece's information is overwritten.
		 * 
		 * \param pt the piece type.
		 * \param c the color.
		 * \param from where the piece is moving from.
		 * \param to where the piece is moving to.
		 * \param capture whether this piece is capturing another piece.
		 */
		void update(PieceType pt, Color c, int from, int to, bool capture = false);
		
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
		
		/// \brief Alternate square-centric board representation.
		std::array<Piece, 64> squares;
};
