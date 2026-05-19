#include <cstdint>
#include <array>
#include <string>

class Board {
	public:
		enum Color {
			cWhite,
			cBlack
		};
		
		enum PieceType {
			White,
			Black,
			Pawn,
			Knight,
			Bishop,
			Rook,
			Queen,
			King
		};
		
		// Creates a board with the default configuration
		Board();
		
		// Creates board by copying custom configuration
		Board(std::array<uint64_t, 8> new_pieces);
		
		uint64_t get_piece_set(PieceType pt) const;
		uint64_t get_piece_set(Color c) const;
		uint64_t get_piece_set(PieceType pt, Color c) const;
		
		std::string to_string() const;
		
	private:
		std::array<uint64_t, 8> pieces;
};
