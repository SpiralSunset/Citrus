#include "citrus/search/Searcher.h"
#include "citrus/search/Evaluation.h"
#include "citrus/board/Bitboard.h"
#include <algorithm>
#include <cstdlib>

bool Searcher::is_mate_score(int score) {
	return std::abs(std::abs(score) - MATE) < MAX_SEARCH_PLY; 
}

bool Searcher::time_limit_reached() {
	auto elapsed = std::chrono::steady_clock::now() - start_time;
	if (elapsed >= std::chrono::milliseconds(settings.movetime)) return true;
	return false;
}

int Searcher::move_order_score(Position &pos, Move move, Move tt_best) {
	if (tt_best.is_valid() && move.equals(tt_best)) return 200;
	if ((uint8_t)move.move_type() & (uint8_t)Move::MoveType::Capture) {
		Board::PieceType target = pos.get_board().get_square(move.to()).pt;
		Board::PieceType capturer = pos.get_board().get_square(move.from()).pt;
		return 100 + mvv_lva[target-2][capturer-2];
	}
	return 0;
}

Move Searcher::pick_move(Position &pos, MoveList &move_list, int move_idx, int ply, Move tt_best) {
	// PV moves should always be the highest priority
	if (move_idx == 0) {
		for (int i = 0; i < move_list.get_size(); i++) {
			if (move_list[i].equals(previous_pv[ply])) {
				std::swap(move_list[0], move_list[i]);
				return move_list[move_idx];
			}
		}
	}
	
	int best = move_idx;
	int best_score = move_order_score(pos, move_list[move_idx], tt_best);
	for (int i = move_idx + 1; i < move_list.get_size(); i++) {
		int score = move_order_score(pos, move_list[i], tt_best);
		if (score > best_score) {
			best = i;
			best_score = score;
		}
	}
	
	std::swap(move_list[move_idx], move_list[best]);
	
	return move_list[move_idx];
}

Move Searcher::find_best_move(Position pos) {
	stopping = false;
	node_count = 0;
	start_time = std::chrono::steady_clock::now();
	
	SearchResult sr;
	// Iterative deepening, a search is done with iterating depths
	// This is actually faster than a single search, since ttable and principle variation are retained
	for (int d = 1; d <= settings.depth; d++) {
		SearchResult new_sr = negamax(pos, d, 0, -INF, INF);
		
		// Doesn't use the iteration if it wasn't completed
		if (stopping) break;
		sr = new_sr;
		
		// Copies the root pv from the pv table to previous_pv
		for (int i = 0; i < pv_length[0]; i++) {
			previous_pv[i] = pv_table[0][i];
		}
		previous_pv_length = pv_length[0];
	}
	return sr.chosen_move;
}

Searcher::SearchResult Searcher::quiesce(Position &pos, int alpha, int beta, int ply) {
	SearchResult out;
	node_count++;
	out.node = node_count;

	if (pos.has_threefold_repetition()) {
		out.score = 0;
		return out;
	}

	// Since there is no depth limit here, the search could theoretically reach the max number of plies.
	// It is incredibly unlikely that this will ever happen, but I have accounted for it anyway.
	if (ply >= MAX_SEARCH_PLY - 1) {
		out.score = Evaluation::evaluate(pos);
		return out;
	}

	bool in_check = move_generator.is_side_in_check(pos, Move::MoveType::Quiet, pos.get_side_to_move());

	if (!in_check) {
		out.score = Evaluation::evaluate(pos);
		if (out.score >= beta) {
			return out;
		} else if (out.score > alpha) {
			alpha = out.score;
		}
	} else {
		out.score = -INF;
	}

	bool could_move = false;
	move_generator.gen_pseudo_legal_moves(pos, list_stack[ply]);
	for (int ml_idx = 0; ml_idx < list_stack[ply].get_size(); ml_idx++) {
		//Move move = list_stack[ply][ml_idx];
		Move move = pick_move(pos, list_stack[ply], ml_idx, ply, Move());
		//if (!in_check && !(((uint8_t)move.move_type() & (uint8_t)Move::MoveType::Capture) || ((uint8_t)move.move_type() & (uint8_t)Move::MoveType::KnightPromotion))) continue; // Skip non-capture moves
		if (!in_check && (uint8_t)move.move_type() <= (uint8_t)Move::MoveType::QueenCastle) continue; // Skip non-tactical moves

		pos.make_move(move);

		if (!move_generator.is_side_in_check(pos, move.move_type(), pos.get_next_to_move())) {
			could_move = true;
			SearchResult result = quiesce(pos, -beta, -alpha, ply+1);
			result.score = -result.score;
			pos.unmake_move(move);

			if (result.score >= beta) {
				out = result;
				out.chosen_move = move;
				return out;
			}
			if (result.score > out.score) {
				out.score = result.score;
				out.chosen_move = move;
			}
			if (result.score > alpha) {
				alpha = result.score;
			}
		} else {
			pos.unmake_move(move);
		}
	}

	// Can't use could_move for finding stalemate here because it represents TACTICAL moves
	if (!could_move && in_check) {
		out.score = -MATE + ply; // Checkmate
	}

	return out;
}

Searcher::SearchResult Searcher::negamax(Position &pos, int depth, int ply, int alpha, int beta) {
	SearchResult out;
	node_count++;
	pv_length[ply] = ply;
	
	if (settings.finite_nodes && node_count >= settings.max_nodes) stopping = true;
	
	if (settings.finite_time && (node_count % 2048 == 0) && time_limit_reached()) stopping = true;

	if (pos.has_threefold_repetition()) {
		out.score = 0;
		return out;
	}
	
	// Base case
	if (depth == 0 || stopping) {
		SearchResult quiet = quiesce(pos, alpha, beta, ply);
		return quiet;
	}
	
	// Determines if the ttable should be used
	uint64_t key = pos.get_zobrist_key();
	TTable::Entry entry = ttable[key];
	Move tt_best;
	bool key_match = entry.key == key;
	if (key_match) {
		if (entry.depth >= depth) { 
			if ((entry.bound == TTable::Bound::Exact) ||
			    (entry.bound == TTable::Bound::Lower && entry.score >= beta) ||
			    (entry.bound == TTable::Bound::Upper && entry.score <= alpha)) {
				// Perform cutoff
				if (is_mate_score(entry.score)) { // Mate scores always must be relative to ply
					if (entry.score > 0) {
						out.score = entry.score - ply;
					} else {
						out.score = entry.score + ply;
					}
				} else {
					out.score = entry.score;
				}
				out.chosen_move = entry.best_move;
				out.node = node_count;
				return out;
			}
		}
		tt_best = entry.best_move;
	}
	int org_alpha = alpha; // Used later when adding entries to the ttable
	
	// The search
	int max = -INF;
	move_generator.gen_pseudo_legal_moves(pos, list_stack[ply]);
	bool could_move = false;
	int move_idx = 0;
	while (move_idx < list_stack[ply].get_size()) {
		Move move = pick_move(pos, list_stack[ply], move_idx, ply, tt_best);
		move_idx++;
		pos.make_move(move);

		if (!move_generator.is_side_in_check(pos, move.move_type(), pos.get_next_to_move())) {
			could_move = true;
			
			SearchResult result = negamax(pos, depth-1, ply+1, -beta, -alpha);
			result.score = -result.score;
						
			if (result.score > max) {
				max = result.score;
				out = result;
				out.chosen_move = move;
				if (result.score > alpha) {
					alpha = result.score;
					
					pv_table[ply][ply] = move; // Top of pv is the new best move
					for (int pv_idx = ply + 1; pv_idx < pv_length[ply + 1]; pv_idx++) { // Rest of pv is copied to next ply
						pv_table[ply][pv_idx] = pv_table[ply + 1][pv_idx];
					}
					pv_length[ply] = pv_length[ply + 1]; // Updates length of pv in next ply
				}
			}
			if (alpha >= beta) {
				pos.unmake_move(move);
				break;
			}
		}
		
		pos.unmake_move(move);
	}
	
	// Determines checkmate or stalemate when unable to make a legal move
	// IMPORTANT: out.best_move is never initialized in this case
	if (!could_move) {
		if (move_generator.is_side_in_check(pos, Move::MoveType::Quiet, pos.get_side_to_move())) {
			out.score = -MATE + ply; // Checkmate
		} else {
			out.score = 0; // Stalemate
		}
	}
	
	// Writes new entry to the ttable unless the existing entry has a greater depth
	if (!stopping && (!entry.is_valid || depth >= entry.depth)) {
		TTable::Entry new_entry;
		new_entry.key = key;
		new_entry.best_move = out.chosen_move;
		new_entry.depth = depth;
		new_entry.score = out.score;
		// Normalizes mate score
		if (is_mate_score(out.score)) {
			if (out.score > 0) {
				new_entry.score += ply;
			} else {
				new_entry.score -= ply;
			}
		}
		if (out.score >= beta) new_entry.bound = TTable::Bound::Lower;
		else if (out.score <= org_alpha) new_entry.bound = TTable::Bound::Upper;
		else new_entry.bound = TTable::Bound::Exact;
		new_entry.is_valid = true;
		ttable[key] = new_entry;
	}
	
	return out;
}
