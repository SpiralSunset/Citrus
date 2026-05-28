#include "citrus/board/Position.h"
#include <algorithm>

Position::Position() {
	side_to_move = Board::Color::cWhite;
	castling_rights.fill(true);
	ep_square = -1;
	halfmove_clock = 0;
	fullmove_counter = 0;
}

Position::Position(Board new_board, Board::Color new_side_to_move, std::array<bool, 4> new_castling_rights, int new_ep_square, int new_halfmove_clock, int new_fullmove_counter) {
	board = new_board;
	side_to_move = new_side_to_move;
	std::copy(new_castling_rights.begin(), new_castling_rights.end(), castling_rights.begin());
	ep_square = new_ep_square;
	halfmove_clock = new_halfmove_clock;
	fullmove_counter = new_fullmove_counter;
}


Board Position::get_board() const {
	return board;
}

Board::Color Position::get_side_to_move() const {
	return side_to_move;
}

bool Position::get_castling_right(Position::CastlingRight cr) const {
	return castling_rights[(uint8_t)cr];
}

int Position::get_ep_square() const {
	return ep_square;
}

int Position::get_halfmove_clock() const {
	return halfmove_clock;
}

int Position::get_fullmove_counter() const {
	return fullmove_counter;
}

void Position::make_move(Move move) {
	Board::Piece moving = board.get_square(move.from());
	int new_ep_square = -1;
	bool is_capture = ((uint8_t)move.move_type() & (uint8_t)Move::MoveType::Capture);
	
	// Castling rights checker
	if (moving.pt == Board::PieceType::King) {
		if (moving.c == Board::Color::cWhite && move.from() == 4) {
			castling_rights[(uint8_t)Position::CastlingRight::wKing] = false;
			castling_rights[(uint8_t)Position::CastlingRight::wQueen] = false;
		} else if (moving.c == Board::Color::cBlack && move.from() == 60) {
			castling_rights[(uint8_t)Position::CastlingRight::bKing] = false;
			castling_rights[(uint8_t)Position::CastlingRight::bQueen] = false;
		}
	} else if (moving.pt == Board::PieceType::Rook) {
		if (moving.c == Board::Color::cWhite) {
			if (move.from() == 0) castling_rights[(uint8_t)Position::CastlingRight::wQueen] = false;
			else if (move.from() == 7)  castling_rights[(uint8_t)Position::CastlingRight::wKing] = false;
		} else if (moving.c == Board::Color::cBlack) {
			if (move.from() == 56) castling_rights[(uint8_t)Position::CastlingRight::bQueen] = false;
			else if (move.from() == 63)  castling_rights[(uint8_t)Position::CastlingRight::bKing] = false;
		}
	}
	
	// Board updating
	if ((uint8_t)move.move_type() & (uint8_t)Move::MoveType::KnightPromotion) { // Promotion behavior
		board.remove_piece(moving.pt, side_to_move, move.from());
		if (is_capture) {
			Board::Piece target = board.get_square(move.to());
			board.remove_piece(target.pt, target.c, move.to());
		} 
		board.add_piece(static_cast<Board::PieceType>(Board::PieceType::Knight + ((uint8_t)move.move_type() & 0x3)), side_to_move, move.to());
	} else if ((uint8_t)move.move_type() == (uint8_t)Move::MoveType::EpCapture) { // En passant behavior
		int cap_square = move.to() + (side_to_move == Board::Color::cWhite ? -8 : 8);
		Board::Piece cap_target = board.get_square(cap_square);
		board.remove_piece(cap_target.pt, cap_target.c, cap_square);
		board.update(moving.pt, side_to_move, move.from(), move.to());
	} else { // Every other kind of move
		switch (move.move_type()) {
			case Move::MoveType::DoublePawnPush:
				new_ep_square = move.to() - (side_to_move == Board::Color::cWhite ? 8 : -8);
				break;
			case Move::MoveType::KingCastle:
				board.update(Board::PieceType::Rook, side_to_move, move.from() + 3, move.to() - 1);
				break;
			case Move::MoveType::QueenCastle:
				board.update(Board::PieceType::Rook, side_to_move, move.from() - 4, move.to() + 1);
				break;
		}
		board.update(moving.pt, side_to_move, move.from(), move.to(), is_capture);
	}
	
	// Switching side and incrementing the fullmove counter after black turns
	if (side_to_move == Board::Color::cWhite) {
		side_to_move = Board::Color::cBlack;
	} else if (side_to_move == Board::Color::cBlack) {
		side_to_move = Board::Color::cWhite;
		fullmove_counter++;
	}
	
	// Incrementing or resetting the halfmove clock
	if (moving.pt == Board::PieceType::Pawn || is_capture) {
		halfmove_clock = 0;
	} else {
		halfmove_clock++;
	}
	
	// Clearing or updating the ep target square
	ep_square = new_ep_square;
	
}
