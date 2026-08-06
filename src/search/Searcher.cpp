#include "citrus/search/Searcher.h"
#include "citrus/search/Evaluation.h"
#include "citrus/board/Bitboard.h"
#include <iostream>
#include <cstdlib>

bool Searcher::is_mate_score(int score) {
	return std::abs(score - MATE) < MAX_SEARCH_PLY; 
}

bool Searcher::time_limit_reached() {
	auto elapsed = std::chrono::steady_clock::now() - start_time;
	if (elapsed >= std::chrono::milliseconds(settings.movetime)) return true;
	return false;
}

Move Searcher::find_best_move(Position pos) {
	stopping = false;
	node_count = 0;
	start_time = std::chrono::steady_clock::now();
	SearchResult sr = negamax(pos, settings.depth, -INF, INF);
	return sr.chosen_move;
}

Searcher::SearchResult Searcher::negamax(Position &pos, int depth, int alpha, int beta) {
	SearchResult out;
	node_count++;
	
	if (settings.finite_nodes && node_count >= settings.max_nodes) stopping = true;
	
	if (settings.finite_time && (node_count % 2048 == 0) && time_limit_reached()) stopping = true;
	
	if (depth == 0 || stopping) {
		out.score = Evaluation::evaluate(pos);
		out.node = node_count;
		return out;
	}
	
	int max = -INF;
	move_generator.gen_pseudo_legal_moves(pos, list_stack[depth]);
	bool could_move = false;
	for (int i = 0; i < list_stack[depth].get_size(); i++) {
		pos.make_move(list_stack[depth][i]);

		if (!move_generator.is_side_in_check(pos, list_stack[depth][i].move_type(), pos.get_next_to_move())) {
			could_move = true;
			
			SearchResult result = negamax(pos, depth-1, -beta, -alpha);
			result.score = -result.score;
			result.chosen_move = list_stack[depth][i];
						
			if (result.score > max) {
				max = result.score;
				out = result;
				if (result.score > alpha) alpha = result.score;
			}
			if (result.score >= beta || stopping) {
				pos.unmake_move(list_stack[depth][i]);
				return result;
			}
		}
		
		pos.unmake_move(list_stack[depth][i]);
	}
	
	if (!could_move) {
		if (move_generator.is_side_in_check(pos, Move::MoveType::Quiet, pos.get_side_to_move())) {
			out.score = -MATE + depth; // Checkmate
		} else {
			out.score = 0; // Stalemate
		}
	}
	
	return out;
}
