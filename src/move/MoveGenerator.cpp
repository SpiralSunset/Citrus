#include "citrus/move/MoveGenerator.h"
#include "citrus/board/Bitboard.h"
#include <iostream>

MoveGenerator::MoveGenerator() {
	uint64_t sq_bb = 1ULL;
	for (int sq = 0; sq < 64; ++sq, sq_bb <<= 1) {
		knight_moves[sq] = 
			(sq_bb << 17 & not_FILE_A)  |
			(sq_bb << 15 & not_FILE_H)  |
			(sq_bb << 10 & not_FILE_AB) |
			(sq_bb << 6 & not_FILE_GH)  |
			(sq_bb >> 6 & not_FILE_AB)  |
			(sq_bb >> 10 & not_FILE_GH) |
			(sq_bb >> 15 & not_FILE_A)  |
			(sq_bb >> 17 & not_FILE_H);
			
		king_moves[sq] =
			(sq_bb << 9 & not_FILE_A) |
			(sq_bb << 8)              |
			(sq_bb << 7 & not_FILE_H) |
			(sq_bb << 1 & not_FILE_A) |
			(sq_bb >> 1 & not_FILE_H) |
			(sq_bb >> 7 & not_FILE_A) |
			(sq_bb >> 8)              |
			(sq_bb >> 9 & not_FILE_H);
			
		pawn_attacks[0][sq] =
			(sq_bb << 7 & not_FILE_H) |
			(sq_bb << 9 & not_FILE_A);
			
		pawn_attacks[1][sq] =
			(sq_bb >> 7 & not_FILE_A) |
			(sq_bb >> 9 & not_FILE_H);
	}
}

uint64_t MoveGenerator::slow_ray_attacks(int sq, uint64_t occ, int df, int dr) {
	uint64_t attacks = 0ULL;
	
	int file = (sq % 8) + df;
	int rank = (sq / 8) + dr;
	
	while (file >= 0 && file < 8 && rank >= 0 && rank < 8) {
		int n_sq = rank * 8 + file;
		attacks |= 1ULL << n_sq;
		if (occ & (1ULL << n_sq)) break;
		
		file += df;
		rank += dr;
	}
	
	return attacks;
}

uint64_t MoveGenerator::get_bishop_attacks(int sq, uint64_t occ) {
	return slow_ray_attacks(sq, occ, -1, -1) |
	       slow_ray_attacks(sq, occ, -1, 1)  |
	       slow_ray_attacks(sq, occ, 1, -1)  |
	       slow_ray_attacks(sq, occ, 1, 1);
}

uint64_t MoveGenerator::get_rook_attacks(int sq, uint64_t occ) {
	return slow_ray_attacks(sq, occ, -1, 0) |
	       slow_ray_attacks(sq, occ, 1, 0)  |
	       slow_ray_attacks(sq, occ, 0, -1) |
	       slow_ray_attacks(sq, occ, 0, 1);
}

void MoveGenerator::append_moves(MoveList &moves, int sq, uint64_t targets, Move::MoveType move_type) {
	while (targets) {
		int to = pop_lsb(targets);
		if ((uint8_t)move_type >= (uint8_t)Move::MoveType::KnightPromotionCapture) {
			moves.add(Move(sq, to, Move::MoveType::KnightPromotionCapture));
			moves.add(Move(sq, to, Move::MoveType::BishopPromotionCapture));
			moves.add(Move(sq, to, Move::MoveType::RookPromotionCapture));
			moves.add(Move(sq, to, Move::MoveType::QueenPromotionCapture));
		} else if ((uint8_t)move_type >= (uint8_t)Move::MoveType::KnightPromotion) {
			moves.add(Move(sq, to, Move::MoveType::KnightPromotion));
			moves.add(Move(sq, to, Move::MoveType::BishopPromotion));
			moves.add(Move(sq, to, Move::MoveType::RookPromotion));
			moves.add(Move(sq, to, Move::MoveType::QueenPromotion));
		} else {
			moves.add(Move(sq, to, move_type));
		}
	}
}

void MoveGenerator::gen_pseudo_legal_moves(Position &pos, MoveList &moves) {
	moves.clear();
	
	uint64_t friendly_occ = pos.get_board().get_piece_set(pos.get_side_to_move());
	uint64_t opposing_occ = pos.get_board().get_piece_set(pos.get_next_to_move());
	uint64_t occ = friendly_occ | opposing_occ;
	
	// Pawns
	uint64_t pawns = pos.get_board().get_piece_set(Board::PieceType::Pawn, pos.get_side_to_move());
	while (pawns) {
		int sq = pop_lsb(pawns);
		uint64_t sq_bb = 1ULL << sq;
		uint64_t single_push = 0ULL;
		uint64_t double_push = 0ULL;
		uint64_t promotion = 0ULL;
		uint64_t captures = 0ULL;
		uint64_t promotion_captures = 0ULL;
		uint64_t ep_capture = 0ULL;
		if (pos.get_side_to_move() == Board::Color::cWhite) {
			single_push = ((sq_bb << 8) & ~occ) & ~RANK_8;
			double_push = ((((sq_bb & RANK_2) << 8) & ~occ) << 8) & ~occ;
			promotion = ((sq_bb << 8) & ~occ) & RANK_8;
			captures = pawn_attacks[Board::Color::cWhite][sq] & pos.get_board().get_piece_set(Board::Color::cBlack) & ~RANK_8;
			promotion_captures = pawn_attacks[Board::Color::cWhite][sq] & pos.get_board().get_piece_set(Board::Color::cBlack) & RANK_8;
			if (pos.get_ep_square() != -1) ep_capture = pawn_attacks[Board::Color::cWhite][sq] & (1ULL << pos.get_ep_square());
		} else {
			single_push = ((sq_bb >> 8) & ~occ) & ~RANK_1;
			double_push = ((((sq_bb & RANK_7) >> 8) & ~occ) >> 8) & ~occ;
			promotion = ((sq_bb >> 8) & ~occ) & RANK_1;
			captures = pawn_attacks[Board::Color::cBlack][sq] & pos.get_board().get_piece_set(Board::Color::cWhite) & ~RANK_1;
			promotion_captures = pawn_attacks[Board::Color::cBlack][sq] & pos.get_board().get_piece_set(Board::Color::cWhite) & RANK_1;
			if (pos.get_ep_square() != -1) ep_capture = pawn_attacks[Board::Color::cBlack][sq] & (1ULL << pos.get_ep_square());
		}
		append_moves(moves, sq, single_push, Move::MoveType::Quiet);
		append_moves(moves, sq, double_push, Move::MoveType::DoublePawnPush);
		append_moves(moves, sq, promotion, Move::MoveType::KnightPromotion);
		append_moves(moves, sq, captures, Move::MoveType::Capture);
		append_moves(moves, sq, promotion_captures, Move::MoveType::KnightPromotionCapture);
		append_moves(moves, sq, ep_capture, Move::MoveType::EpCapture);
	}
	
	// Knights
	uint64_t knights = pos.get_board().get_piece_set(Board::PieceType::Knight, pos.get_side_to_move());
	while (knights) {
		int sq = pop_lsb(knights);
		append_moves(moves, sq, knight_moves[sq] & ~occ, Move::MoveType::Quiet);
		append_moves(moves, sq, knight_moves[sq] & opposing_occ, Move::MoveType::Capture);
	}
	
	// Bishops
	uint64_t bishops = pos.get_board().get_piece_set(Board::PieceType::Bishop, pos.get_side_to_move());
	while (bishops) {
		int sq = pop_lsb(bishops);
		uint64_t attacks = get_bishop_attacks(sq, occ) & ~friendly_occ;
		append_moves(moves, sq, attacks & ~occ, Move::MoveType::Quiet);
		append_moves(moves, sq, attacks & opposing_occ, Move::MoveType::Capture);
	}
	
	// Rooks
	uint64_t rooks = pos.get_board().get_piece_set(Board::PieceType::Rook, pos.get_side_to_move());
	while (rooks) {
		int sq = pop_lsb(rooks);
		uint64_t attacks = get_rook_attacks(sq, occ) & ~friendly_occ;
		append_moves(moves, sq, attacks & ~occ, Move::MoveType::Quiet);
		append_moves(moves, sq, attacks & opposing_occ, Move::MoveType::Capture);
	}
	
	// Queens
	uint64_t queens = pos.get_board().get_piece_set(Board::PieceType::Queen, pos.get_side_to_move());
	while (queens) {
		int sq = pop_lsb(queens);
		uint64_t attacks = (get_bishop_attacks(sq, occ) | get_rook_attacks(sq, occ)) & ~friendly_occ;
		append_moves(moves, sq, attacks & ~occ, Move::MoveType::Quiet);
		append_moves(moves, sq, attacks & opposing_occ, Move::MoveType::Capture);
	}
	
	// King
	uint64_t king = pos.get_board().get_piece_set(Board::PieceType::King, pos.get_side_to_move());
	while (king) {
		int king_sq = pop_lsb(king);
		append_moves(moves, king_sq, king_moves[king_sq] & ~occ, Move::MoveType::Quiet);
		append_moves(moves, king_sq, king_moves[king_sq] & opposing_occ, Move::MoveType::Capture);
		if (pos.get_side_to_move() == Board::Color::cWhite) {
			if (pos.get_castling_right(Position::CastlingRight::wKing) && !(occ & 0x0000000000000060)) {
				moves.add(Move(king_sq, king_sq+2, Move::MoveType::KingCastle));
			}
			if (pos.get_castling_right(Position::CastlingRight::wQueen) && !(occ & 0x000000000000000E)) {
				moves.add(Move(king_sq, king_sq-2, Move::MoveType::QueenCastle));
			}
		} else {
			if (pos.get_castling_right(Position::CastlingRight::bKing) && !(occ & 0x6000000000000000)) {
				moves.add(Move(king_sq, king_sq+2, Move::MoveType::KingCastle));;
			}
			if (pos.get_castling_right(Position::CastlingRight::bQueen) && !(occ & 0x0E00000000000000)) {
				moves.add(Move(king_sq, king_sq-2, Move::MoveType::QueenCastle));;
			}
		}
	}
}

bool MoveGenerator::is_side_in_check(Position &pos, Move::MoveType last_move_type, Board::Color side) {
	uint64_t king_bb;
	uint64_t occ = pos.get_board().get_piece_set(Board::Color::cWhite) | pos.get_board().get_piece_set(Board::Color::cBlack);
	
	Board::Color opp_side;
	if (side == Board::Color::cWhite) opp_side = Board::Color::cBlack;
	else opp_side = Board::Color::cWhite;
	
	// Increases the spaces being evaluated in accordance to castling rules
	if (last_move_type == Move::MoveType::KingCastle) {
		if (side == Board::Color::cWhite) {
			king_bb = 0x0000000000000070;
		} else {
			king_bb = 0x7000000000000000;
		}
	} else if (last_move_type == Move::MoveType::QueenCastle) {
		if (side == Board::Color::cWhite) {
			king_bb = 0x000000000000001E;
		} else {
			king_bb = 0x1E00000000000000;
		}
	} else {
		king_bb = pos.get_board().get_piece_set(Board::PieceType::King, side);
	}
	
	uint64_t pawns = pos.get_board().get_piece_set(Board::PieceType::Pawn, opp_side);
	while (pawns) {
		int sq = pop_lsb(pawns);
		if (king_bb & pawn_attacks[opp_side][sq]) return true;
	}
	
	uint64_t knights = pos.get_board().get_piece_set(Board::PieceType::Knight, opp_side);
	while (knights) {
		int sq = pop_lsb(knights);
		if (king_bb & knight_moves[sq]) return true;
	}

	uint64_t king = pos.get_board().get_piece_set(Board::PieceType::King, opp_side);
	while (king) {
		int sq = pop_lsb(king);
		if (king_bb & king_moves[sq]) return true;
	}
	
	uint64_t queens = pos.get_board().get_piece_set(Board::PieceType::Queen, opp_side);
	
	uint64_t diagonal_sliding = pos.get_board().get_piece_set(Board::PieceType::Bishop, opp_side) | queens;
	while (diagonal_sliding) {
		int sq = pop_lsb(diagonal_sliding);
		if (king_bb & get_bishop_attacks(sq, occ)) return true;
	}
	
	uint64_t orthagonal_sliding = pos.get_board().get_piece_set(Board::PieceType::Rook, opp_side) | queens;
	while (orthagonal_sliding) {
		int sq = pop_lsb(orthagonal_sliding);
		if (king_bb & get_rook_attacks(sq, occ)) return true;
	}
	
	return false;
}
