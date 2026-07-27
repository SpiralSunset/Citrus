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

Board::Color Position::get_next_to_move() const {
	if (side_to_move == Board::Color::cWhite) return Board::Color::cBlack;
	return Board::Color::cWhite;
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
	Board::Piece cap_target = board.get_square(move.to());
	
	// Initializing the information to be stored in the history
	UndoInfo undo_info;
	undo_info.castling_rights = castling_rights;
	undo_info.ep_square = ep_square;
	undo_info.halfmove_clock = halfmove_clock;
	
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
	} else if (move.move_type() == Move::MoveType::EpCapture) { // En passant behavior
		int cap_square = move.to() + (side_to_move == Board::Color::cWhite ? -8 : 8);
		cap_target = board.get_square(cap_square);
		board.remove_piece(cap_target.pt, cap_target.c, cap_square);
		board.update(moving.pt, side_to_move, move.from(), move.to());
	} else { // Every other kind of move
		switch (move.move_type()) {
			case Move::MoveType::DoublePawnPush:
				new_ep_square = move.to() + (side_to_move == Board::Color::cWhite ? -8 : 8);
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
	
	// Finalizing and pushing the information to be stored in the move history
	undo_info.cap_target = cap_target;
	history.push(undo_info);
	
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

void Position::unmake_move(Move move) {
	UndoInfo undo_info = history.top();
	history.pop();
	
	Board::Piece moving = board.get_square(move.to());
	
	// Reverts piece movement on board
	int cap_square = move.to();
	if ((uint8_t)move.move_type() & (uint8_t)Move::MoveType::KnightPromotion) {
		board.remove_piece(moving.pt, moving.c, move.to());
		board.add_piece(Board::PieceType::Pawn, moving.c, move.from());
	} else {
		switch(move.move_type()) {
			case Move::MoveType::KingCastle:
				board.update(Board::PieceType::Rook, moving.c, move.to() - 1, move.from() + 3);
				break;
			case Move::MoveType::QueenCastle:
				board.update(Board::PieceType::Rook, moving.c, move.to() + 1, move.from() - 4);
				break;
			case Move::MoveType::EpCapture:
				cap_square += (moving.c == Board::Color::cWhite ? -8 : 8);
				break;
		}
		board.update(moving.pt, moving.c, move.to(), move.from());
	}

	// Restores captured piece if there was one
	if ((uint8_t)move.move_type() & (uint8_t)Move::MoveType::Capture) {
		board.add_piece(undo_info.cap_target.pt, undo_info.cap_target.c, cap_square);
	}
	
	// Reverts the rest of the position information
	if (side_to_move == Board::Color::cWhite) {
		side_to_move = Board::Color::cBlack;
		fullmove_counter--;
	} else if (side_to_move == Board::Color::cBlack) {
		side_to_move = Board::Color::cWhite;
	}
	castling_rights = undo_info.castling_rights;
	ep_square = undo_info.ep_square;
	halfmove_clock = undo_info.halfmove_clock;
}
