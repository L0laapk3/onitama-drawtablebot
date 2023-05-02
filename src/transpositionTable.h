#pragma once

#include "types.h"
#include "board.h"

#include <array>





struct Transposition {
	Depth depth;
	Move move;
	Score score;
	U64 hash;
};


struct TranspositionEntry {
	Transposition depthPreferred;
	Transposition replaceAlways;
};

// 2^23 seems to be the smallest size that works well for a 100ms search
constexpr U64 TT_SIZE = (1ULL << 29) / sizeof(Transposition);



class TranspositionTableWrapper {
public:
	std::vector<TranspositionEntry> table;

	TranspositionTableWrapper();

	bool get(U64 hash, const Transposition*& result) const;
	void put(Transposition transposition);

	void markRecalculate();
	void markRecalculateScore(bool newSearchWin);
};