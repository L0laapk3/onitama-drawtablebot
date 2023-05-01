#include "transpositionTable.h"



TranspositionTableWrapper::TranspositionTableWrapper() {
	table.resize(TT_SIZE);
}


// clears the depth of all TT entries. This way entries are not relied on but the best move heuristic is still used
void TranspositionTableWrapper::markRecalculate() {
	Transposition* it = &table.front().depthPreferred - 1;
	while (it++ != &table.back().replaceAlways) {
		// if (it->hash) // optional check, but slower
		it->depth = 0; // mark for recalculation
	}
}

void TranspositionTableWrapper::markRecalculateScore(bool newSearchWin) {
	Transposition* it = &table.front().depthPreferred - 1;
	while (it++ != &table.back().replaceAlways) {
		if (it->hash) {
			it->depth = 0; // mark for recalculation
			if (std::abs(it->score) >= SCORE::WIN - DEPTH_MAX - 1 && it->move.bound != Bound::NONE) { // affect all boards that have win states
				it->move.bound = newSearchWin ? Bound::EXACT : it->score > 0 ? Bound::LOWER : Bound::UPPER;
				it->score = (it->score > 0 ? 1 : -1) * (SCORE::WIN - (newSearchWin ? DEPTH_MAX + 1 : 0));
			}
		}
	}
}