#include "transpositionTable.h"



TranspositionTableWrapper::TranspositionTableWrapper() {
	table.resize(TT_SIZE);
}



void TranspositionTableWrapper::markRecalculate() {
	Transposition* it = &table.front().depthPreferred - 1;
	while (it++ != &table.back().replaceAlways) {
		// if (it->hash) // optional check
		it->depth = 0; // mark for recalculation
	}
}