#include "citrus/debug/Perft.h"
#include "citrus/move/MoveList.h"
#include <cstdint>
#include <iostream>

uint64_t Perft::run(Position &pos, MoveGenerator &mg, int depth) {
	std::array<MoveList, MAX_SEARCH_PLY> list_stack;
	return run(pos, mg, list_stack, depth);
}

uint64_t Perft::run(Position &pos, MoveGenerator &mg, std::array<MoveList, MAX_SEARCH_PLY> &list_stack, int depth) {
	uint64_t nodes = 0;
	
	if (depth == 0) return 1ULL;
	
	mg.gen_pseudo_legal_moves(pos, list_stack[depth]);
	int num_moves = list_stack[depth].get_size();
	for (int i = 0; i < num_moves; i++) {
		pos.make_move(list_stack[depth][i]);
		if (!mg.is_side_in_check(pos, list_stack[depth][i].move_type(), pos.get_next_to_move())) {
			nodes += run(pos, mg, list_stack, depth-1);
		}
		pos.unmake_move(list_stack[depth][i]);
	}
	return nodes;
}

void Perft::divide(Position &pos, MoveGenerator &mg, int depth) {
	std::array<MoveList, MAX_SEARCH_PLY> list_stack;
	mg.gen_pseudo_legal_moves(pos, list_stack[depth]);
	int num_moves = list_stack[depth].get_size();
	for (int i = 0; i < num_moves; i++) {
		pos.make_move(list_stack[depth][i]);
		if (!mg.is_side_in_check(pos, list_stack[depth][i].move_type(), pos.get_next_to_move())) {
			std::cout << square_to_uci(list_stack[depth][i].from()) << square_to_uci(list_stack[depth][i].to()) << ": " << run(pos, mg, list_stack, depth-1) << "\n";
		}
		pos.unmake_move(list_stack[depth][i]);
	}
}

std::string Perft::square_to_uci(int sq) {
	std::string str = "";
	str.push_back('a' + (sq % 8));
	str.push_back('1' + (sq / 8));
	return str;
}
