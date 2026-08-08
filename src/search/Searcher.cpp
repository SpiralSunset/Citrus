#include "citrus/search/Searcher.h"
#include "citrus/search/Evaluation.h"
#include "citrus/board/Bitboard.h"
#include <algorithm>
#include <cstdlib>

bool Searcher::is_mate_score(int score) {
	return std::abs(score - MATE) < MAX_SEARCH_PLY; 
}

bool Searcher::time_limit_reached() {
	auto elapsed = std::chrono::steady_clock::now() - start_time;
	if (elapsed >= std::chrono::milliseconds(settings.movetime)) return true;
	return false;
}

int Searcher::move_order_score(Move move) {
	if ((uint8_t)move.move_type() & (uint8_t)Move::MoveType::Capture) return 1;
	return 0;
}

Move Searcher::pick_move(MoveList &move_list, int move_idx, int ply) {
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
	int best_score = move_order_score(move_list[move_idx]);
	for (int i = move_idx + 1; i < move_list.get_size(); i++) {
		int score = move_order_score(move_list[i]);
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

Searcher::SearchResult Searcher::negamax(Position &pos, int depth, int ply, int alpha, int beta) {
	SearchResult out;
	node_count++;
	pv_length[ply] = ply;
	
	if (settings.finite_nodes && node_count >= settings.max_nodes) stopping = true;
	
	if (settings.finite_time && (node_count % 2048 == 0) && time_limit_reached()) stopping = true;
	
	if (depth == 0 || stopping) {
		out.score = Evaluation::evaluate(pos);
		out.node = node_count;
		return out;
	}
	
	int max = -INF;
	move_generator.gen_pseudo_legal_moves(pos, list_stack[ply]);
	bool could_move = false;
	int move_idx = 0;
	while (move_idx < list_stack[ply].get_size()) {
		Move move = pick_move(list_stack[ply], move_idx, ply);
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
			if (alpha >= beta || stopping) {
				pos.unmake_move(move);
				return out;
			}
		}
		
		pos.unmake_move(move);
	}
	
	if (!could_move) {
		if (move_generator.is_side_in_check(pos, Move::MoveType::Quiet, pos.get_side_to_move())) {
			out.score = -MATE + ply; // Checkmate
		} else {
			out.score = 0; // Stalemate
		}
	}
	
	return out;
}
