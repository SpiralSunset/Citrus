#include "citrus/io/Fen.h"
#include <sstream>

std::string Fen::pos_to_str(Position pos) {
	std::string str = "";
	
	std::array<Board::Piece, 64> piece_list = pos.get_board().to_array();
	
	int empty_counter = 0;
	for (int row = 7; row >= 0; --row) {
		for (int i = row * 8; i < (row + 1) * 8; ++i) {
			if (piece_list[i].pt == Board::PieceType::None) empty_counter++;
			else {
				if (empty_counter > 0) {
					str.push_back('0' + empty_counter);
					empty_counter = 0;
				}
				str.push_back(piece_to_char(piece_list[i]));
			}
			if (i % 8 == 7) {
				if (empty_counter > 0) {
					str.push_back('0' + empty_counter);
					empty_counter = 0;
				}
				if (i != 7) str.push_back('/');
			}
		}
	}
	
	str.append(pos.get_side_to_move() == Board::Color::cWhite ? " w " : " b ");
	
	std::string castle_buffer = "";
	if (pos.get_castling_right(Position::CastlingRight::wKing)) castle_buffer.push_back('K');
	if (pos.get_castling_right(Position::CastlingRight::wQueen)) castle_buffer.push_back('Q');
	if (pos.get_castling_right(Position::CastlingRight::bKing)) castle_buffer.push_back('k');
	if (pos.get_castling_right(Position::CastlingRight::bQueen)) castle_buffer.push_back('q');
	if (castle_buffer.empty()) str.append("- ");
	else str.append(castle_buffer + " ");
	
	int ep_square = pos.get_ep_square();
	if (ep_square == -1) str.append("- ");
	else {
		str.push_back('a' + (ep_square % 8)); 
		str.push_back('1' + (ep_square / 8));
		str.push_back(' ');
	}
	
	str.append(std::to_string(pos.get_halfmove_clock()) + " " + std::to_string(pos.get_fullmove_counter()));
	
	return str;
}

Position Fen::str_to_pos(std::string str) {
	std::istringstream iss(str);
	
	std::array<uint64_t, 8> pieces;
	pieces.fill(0);
	
	std::string buffer;
	iss >> buffer;
	
	int sq_idx = 56;
	for (char c : buffer) {
		if (c >= '0' && c <= '8') sq_idx += c - '0';
		else if (c >= 'A' && c <= 'Z') {
			Board::PieceType pt = char_to_piece_type(c);
			uint64_t bb = 1ULL << sq_idx;
			pieces[(int)pt] |= bb;
			pieces[(int)Board::PieceType::White] |= bb;
			sq_idx++;
		} else if (c >= 'a' && c <= 'z') {
			Board::PieceType pt = char_to_piece_type(c - ('a' - 'A'));
			uint64_t bb = 1ULL << sq_idx;
			pieces[(int)pt] |= bb;
			pieces[(int)Board::PieceType::Black] |= bb;
			sq_idx++;
		} else if (c == '/') {
			sq_idx -= 16;
		}
	}
	
	Board board(pieces);
	Position pos(board);
	return pos;
}

char Fen::piece_to_char(Board::Piece p) {
	char out;
	
	switch(p.pt) {
		case Board::PieceType::Pawn:
			out = 'P';
			break;
		case Board::PieceType::Knight:
			out = 'N';
			break;
		case Board::PieceType::Bishop:
			out = 'B';
			break;
		case Board::PieceType::Rook:
			out = 'R';
			break;
		case Board::PieceType::Queen:
			out = 'Q';
			break;
		case Board::PieceType::King:
			out = 'K';
			break;
		default:
			out = '?'; // This should never happen
			return out;
	}
	
	if (p.c == Board::Color::cBlack) {
		out = out + ('a' - 'A');
	}
	return out;
}

Board::PieceType Fen::char_to_piece_type(char c) {
	switch (c) {
		case 'P':
			return Board::PieceType::Pawn;
		case 'N':
			return Board::PieceType::Knight;
		case 'B':
			return Board::PieceType::Bishop;
		case 'R':
			return Board::PieceType::Rook;
		case 'Q':
			return Board::PieceType::Queen;
		case 'K':
			return Board::PieceType::King;
		default:
			return Board::PieceType::None; // This should never happen
	}
}
