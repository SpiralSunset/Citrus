#include "citrus/search/Searcher.h"
#include "citrus/search/Evaluation.h"
#include "citrus/board/Bitboard.h"

int Searcher::get_depth() {
	return depth;
}

void Searcher::set_depth(int depth) {
	if (depth <= 1) this->depth = 1;
	else if (depth >= MAX_SEARCH_PLY) this->depth = MAX_SEARCH_PLY - 1;
	else this->depth = depth;
}

Move Searcher::find_best_move(Position &pos) {
	node_count = 0;
	return negamax(pos, depth, -INF, INF).chosen_move;
}

Searcher::SearchResult Searcher::negamax(Position &pos, int depth, int alpha, int beta) {
	SearchResult out;
	node_count++;
	
	if (depth == 0) {
		out.score = Evaluation::evaluate(pos);
		out.node = node_count;
		return out;
	}
	
	int max = -INF;
	move_generator.gen_pseudo_legal_moves(pos, list_stack[depth]);
	bool could_move = false;
	for (int i = 0; i < list_stack[depth].get_size(); i++) {
		pos.make_move(list_stack[depth][i]);

		if (!move_generator.is_opposing_king_in_check(pos, list_stack[depth][i].move_type())) {
			could_move = true;
			
			SearchResult result = negamax(pos, depth-1, -beta, -alpha);
			result.score = -result.score;
			result.chosen_move = list_stack[depth][i];
						
			if (result.score > max) {
				max = result.score;
				out = result;
				if (result.score > alpha) alpha = result.score;
			}
			if (result.score >= beta) {
				out = result;
				pos.unmake_move(list_stack[depth][i]);
				return out;
			}
		}
		
		pos.unmake_move(list_stack[depth][i]);
	}
	
	return out;
}
