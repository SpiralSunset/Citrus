#include "citrus/board/Position.h"
#include "citrus/board/Bitboard.h"
#include <algorithm>

Position::Position() {
	side_to_move = Board::Color::cWhite;
	castling_rights.fill(true);
	ep_square = -1;
	halfmove_clock = 0;
	fullmove_counter = 0;
	create_hash();
}

Position::Position(Board new_board, Board::Color new_side_to_move, std::array<bool, 4> new_castling_rights, int new_ep_square, int new_halfmove_clock, int new_fullmove_counter) {
	board = new_board;
	side_to_move = new_side_to_move;
	std::copy(new_castling_rights.begin(), new_castling_rights.end(), castling_rights.begin());
	ep_square = new_ep_square;
	halfmove_clock = new_halfmove_clock;
	fullmove_counter = new_fullmove_counter;
	create_hash();
}


const Board& Position::get_board() const {
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

uint64_t Position::get_zobrist_key() const {
	return hash;
}

void Position::create_hash() {
	hash = 0ULL;
	
	for (int i = 0; i < 12; i++) {
		Board::PieceType pt = static_cast<Board::PieceType>((i%6)+2);
		Board::Color c = static_cast<Board::Color>(i/6);
		uint64_t bb = board.get_piece_set(pt, c);
		while (bb) {
			int sq = pop_lsb(bb);
			hash ^= zobrist.get_piece_hash(pt, c, sq);
		}
	}
	
	hash ^= zobrist.get_side_hash(side_to_move);
	
	for (int i = 0; i < 4; i++) {
		if (castling_rights[i]) hash ^= zobrist.get_castle_hash(i); 
	}
	
	if (ep_square != -1) hash ^= zobrist.get_ep_hash(ep_square);
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
	undo_info.hash = hash;
	
	// Castling rights checker
	if (moving.pt == Board::PieceType::King) {
		if (moving.c == Board::Color::cWhite && move.from() == 4) {
			disable_castling_right(Position::CastlingRight::wKing);
			disable_castling_right(Position::CastlingRight::wQueen);
		} else if (moving.c == Board::Color::cBlack && move.from() == 60) {
			disable_castling_right(Position::CastlingRight::bKing);
			disable_castling_right(Position::CastlingRight::bQueen);
		}
	} else if (moving.pt == Board::PieceType::Rook) {
		if (moving.c == Board::Color::cWhite) {
			if (move.from() == 0) disable_castling_right(Position::CastlingRight::wQueen);
			else if (move.from() == 7) disable_castling_right(Position::CastlingRight::wKing);
		} else if (moving.c == Board::Color::cBlack) {
			if (move.from() == 56) disable_castling_right(Position::CastlingRight::bQueen);
			else if (move.from() == 63) disable_castling_right(Position::CastlingRight::bKing);
		}
	}
	
	if (is_capture && cap_target.pt == Board::PieceType::Rook) {
		if (cap_target.c == Board::Color::cWhite) {
			if (move.to() == 0) disable_castling_right(Position::CastlingRight::wQueen);
			else if (move.to() == 7) disable_castling_right(Position::CastlingRight::wKing);
		} else if (cap_target.c == Board::Color::cBlack) {
			if (move.to() == 56) disable_castling_right(Position::CastlingRight::bQueen);
			else if (move.to() == 63) disable_castling_right(Position::CastlingRight::bKing);
		}
	}
	
	// Board updating
	if ((uint8_t)move.move_type() & (uint8_t)Move::MoveType::KnightPromotion) { // Promotion behavior
		remove_piece(moving.pt, side_to_move, move.from());
		if (is_capture) {
			remove_piece(cap_target.pt, cap_target.c, move.to());
		} 
		add_piece(static_cast<Board::PieceType>(Board::PieceType::Knight + ((uint8_t)move.move_type() & 0x3)), side_to_move, move.to());
	} else if (move.move_type() == Move::MoveType::EpCapture) { // En passant behavior
		int cap_square = move.to() + (side_to_move == Board::Color::cWhite ? -8 : 8);
		cap_target = board.get_square(cap_square);
		remove_piece(cap_target.pt, cap_target.c, cap_square);
		update(moving.pt, side_to_move, move.from(), move.to());
	} else { // Every other kind of move
		switch (move.move_type()) {
			case Move::MoveType::DoublePawnPush:
				new_ep_square = move.to() + (side_to_move == Board::Color::cWhite ? -8 : 8);
				break;
			case Move::MoveType::KingCastle:
				update(Board::PieceType::Rook, side_to_move, move.from() + 3, move.to() - 1);
				break;
			case Move::MoveType::QueenCastle:
				update(Board::PieceType::Rook, side_to_move, move.from() - 4, move.to() + 1);
				break;
		}
		update(moving.pt, side_to_move, move.from(), move.to(), is_capture);
	}
	
	// Finalizing and pushing the information to be stored in the move history
	undo_info.cap_target = cap_target;
	history.push(undo_info);
	
	// Switching side and incrementing the fullmove counter after black turns
	hash ^= zobrist.get_side_hash(side_to_move);
	if (side_to_move == Board::Color::cWhite) {
		side_to_move = Board::Color::cBlack;
	} else if (side_to_move == Board::Color::cBlack) {
		side_to_move = Board::Color::cWhite;
		fullmove_counter++;
	}
	hash ^= zobrist.get_side_hash(side_to_move);
	
	// Incrementing or resetting the halfmove clock
	if (moving.pt == Board::PieceType::Pawn || is_capture) {
		halfmove_clock = 0;
	} else {
		halfmove_clock++;
	}
	
	// Clearing or updating the ep target square
	if (ep_square != -1) {
    	hash ^= zobrist.get_ep_hash(ep_square);
	}
	ep_square = new_ep_square;
	if (ep_square != -1) {
		hash ^= zobrist.get_ep_hash(ep_square);
	}
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
	hash = undo_info.hash;
}

void Position::disable_castling_right(CastlingRight cr) {
	uint8_t idx = static_cast<uint8_t>(cr);
    if (!castling_rights[idx]) {
        return;
    }
	castling_rights[(uint8_t)cr] = false;
	hash ^= zobrist.get_castle_hash((uint8_t)cr);
}

void Position::add_piece(Board::PieceType pt, Board::Color c, int sq) {
	board.add_piece(pt, c, sq);
	hash ^= zobrist.get_piece_hash(pt, c, sq);
}
		
void Position::remove_piece(Board::PieceType pt, Board::Color c, int sq) {
	board.remove_piece(pt, c, sq);
	hash ^= zobrist.get_piece_hash(pt, c, sq);
}

void Position::update(Board::PieceType pt, Board::Color c, int from, int to, bool capture) {
	Board::Piece cap_target;
	if (capture) {
		cap_target = board.get_square(to);
	}
	board.update(pt, c, from, to, capture);
	hash ^= zobrist.get_piece_hash(pt, c, from);
	if (capture) {
		hash ^= zobrist.get_piece_hash(cap_target.pt, cap_target.c, to);
	}
	hash ^= zobrist.get_piece_hash(pt, c, to);
}
