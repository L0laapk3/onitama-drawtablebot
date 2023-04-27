#pragma once

#include "types.h"
#include "board.h"

#include <array>

enum NodeType : U8 {
	EXACT      = 0,
	LOWERBOUND = 1,
	UPPERBOUND = 2,
};

struct Transposition {
	U64 hash;
	Score score;
	Depth depth;
	U8 fromBit    : 6;
	U8 secondCard : 1;
	U8 toBit      : 6;
	NodeType type : 2;
};



class TranspositionTableWrapper {
	std::vector<Transposition> TableReplaceAlways;
	std::vector<Transposition> TableDepthPreferred;

public:
	TranspositionTableWrapper();

	Transposition get(U64 hash);
	void put(U64 hash, Transposition& transposition);
};