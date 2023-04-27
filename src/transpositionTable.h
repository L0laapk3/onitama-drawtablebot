#pragma once

#include "types.h"
#include "board.h"

#include <array>


struct Transposition {
	U64 hash;
	Score score;
	Depth depth;
	// TODO bestMove
	// TODO node type
};



class TranspositionTableWrapper {
	std::vector<Transposition> TableReplaceAlways;
	std::vector<Transposition> TableDepthPreferred;

public:
	TranspositionTableWrapper();

	Transposition get(U64 hash);
	void put(U64 hash, Transposition& transposition);
};