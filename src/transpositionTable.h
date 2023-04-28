#pragma once

#include "types.h"
#include "board.h"

#include <array>

enum BoundType : U8 {
	EXACT = 0,
	LOWER = 1,
	UPPER = 2,
};

struct Transposition {
	U64 hash;
	Score score;
	Depth depth;
	U8 fromBit     : 6;
	U8 secondCard  : 1;
	U8 toBit       : 6;
	BoundType type : 2;
};



class TranspositionTableWrapper {
	std::vector<Transposition> TableReplaceAlways;
	std::vector<Transposition> TableDepthPreferred;

public:
	TranspositionTableWrapper();

	Transposition get(U64 hash) const;
	void put(const Transposition transposition);
};