#include "citrus/uci/UCI.h"
#include "citrus/io/Fen.h"

UCI::UCI() : stop_search(false), searcher(stop_search) {};

Move UCI::parse_move(std::string move_str) {
	if (move_str == "0000") {
		Move m;
		return m;
	}
	
	Move::MoveType move_type = Move::MoveType::Quiet;
	
	int from = (move_str.at(0) - 'a') + (move_str.at(1) - '1') * 8;
	int to = (move_str.at(2) - 'a') + (move_str.at(3) - '1') * 8;
	
	Board::Piece from_piece = position.get_board().get_square(from);
	Board::Piece to_piece = position.get_board().get_square(to);
	
	if (from_piece.pt == Board::PieceType::Pawn) {
		int offset = from - to;
		if (offset == 16 || offset == -16) move_type = Move::MoveType::DoublePawnPush;
		else if (to == position.get_ep_square() && offset != 8 && offset != -8) move_type = Move::MoveType::EpCapture;
	}
	if (from_piece.pt == Board::PieceType::King) {
		if (move_str == "e1c1" || move_str == "e8c8") move_type = Move::MoveType::QueenCastle;
		else if (move_str == "e1g1" || move_str == "e8g8") move_type = Move::MoveType::KingCastle;
	}
	if (move_str.size() == 5) {
		switch (move_str.at(4)) {
			case 'n':
				move_type = Move::MoveType::KnightPromotion;
				break;
			case 'b':
				move_type = Move::MoveType::BishopPromotion;
				break;
			case 'r':
				move_type = Move::MoveType::RookPromotion;
				break;
			case 'q':
				move_type = Move::MoveType::QueenPromotion;
				break;
		}
	}
		
	// Sets the capture bit to 1
	if (to_piece.pt != Board::PieceType::None) {
		move_type = static_cast<Move::MoveType>((uint8_t)move_type | (uint8_t)Move::MoveType::Capture);
	}
	return Move(from, to, move_type);
}

void UCI::handle_position(std::istringstream &iss) {
	std::string token;
	iss >> token;
	if (token == "startpos") {
		position = Fen::str_to_pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	} else if (token == "fen") {
		std::string fen = "";
		for (int i = 0; i < 6; i++) {
			iss >> token;
			if (i > 0 && i < 5) fen += " ";
			fen += token;
		}
		position = Fen::str_to_pos(fen);
	}
	iss >> token;
	if (token == "moves") {
		while (iss >> token) {
			Move m = parse_move(token);
			position.make_move(m);
		}
	}
}

void UCI::handle_go(std::istringstream &iss) {
	Searcher::SearchSettings settings;
	MoveList searchmoves_list;
	bool advanced_moves = false;
	
	// Some of these subcommands are not meant to be used together, but that is for the GUI to handle
	std::string token;
	while (iss >> token) {
		if (token == "searchmoves") {
			advanced_moves = true;
			bool iss_empty = false;
			while (true) {
				if (!(iss >> token)) { // Exit both loops if iss is empty
					iss_empty = true;
					break;
				}
				if (token.at(1) < '1' || token.at(1) > '9') break; // Not a move, token is to be used somewhere else
				Move m = parse_move(token);
				searchmoves_list.add(m);
				position.make_move(m);
			}
			if (iss_empty) break;
		}
		if (token == "ponder") {
			// Nothing... yet
		} else if (token == "infinite") {
			settings.depth = Searcher::MAX_SEARCH_PLY-1;
		} else {
			std::string param_str;
			if (!(iss >> param_str)) break;
			int param = std::stoi(param_str);
			if (token == "wtime") {
				settings.wtime = param;
			} else if (token == "btime") {
				settings.btime = param;
			} else if (token == "winc") {
				settings.winc = param;
			} else if (token == "binc") {
				settings.binc = param;
			} else if (token == "movestogo") {
				settings.movestogo = param;
			} else if (token == "movetime") {
				settings.finite_time = true;
				settings.movetime = param;
			} else if (token == "depth") {
				settings.depth = param;
			} else if (token == "nodes") {
				settings.finite_nodes = true;
				settings.max_nodes = param;
			} else if (token == "mate") {
				// Nothing... yet
			}
		}
	}
	
	searcher.settings = settings;
	
	// Starts the search thread and outputs the result when it's done
	search_thread = std::thread([this] {
		best_move = searcher.find_best_move(position);
		std::cout << "bestmove " << best_move.to_uci() << std::endl;
	});
	
	// Restores modified position
	if (advanced_moves) {
		for (int i = searchmoves_list.get_size() - 1; i >= 0; i--) {
			position.unmake_move(searchmoves_list[i]);
		}
	}
}

void UCI::run() {
	std::string input;
	
	while (std::getline(std::cin, input)) {
		if (input == "uci") {
			std::cout << "id name Citrus InDev\n";
			std::cout << "id author Joseph Witte\n";
			std::cout << "uciok" << std::endl;

			while (std::getline(std::cin, input)) {
				std::istringstream iss(input);
				std::string cmd;
				iss >> cmd;
				
				if (cmd == "isready") {
					std::cout << "readyok" << std::endl;
				} else if (cmd == "position") {
					handle_position(iss);
				} else if (cmd == "go") {
					// Ends an ongoing search if there is one first
					if (search_thread.joinable()) {
						stop_search = true;
						search_thread.join();
					}
					stop_search = false;
					handle_go(iss);
				} else if (cmd == "stop") {
					stop_search = true;
					search_thread.join();
				} else if (cmd == "quit") {
					exit(EXIT_SUCCESS);
				}
			}
		}
	}
}
