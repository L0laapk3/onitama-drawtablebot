#pragma once

#include "types.h"
#include "board.h"

#include <array>




enum BoundType : U8 {
	EXACT = 0,
	LOWER = 1,
	UPPER = 2,
};

struct TranspositionMove {
	union {
		U16 full;
		struct {
			union {
				U8 fromBitFull;
				struct {
					U8 fromBit     : 6;
					BoundType type : 2;
				};
			};
			union {
				U8 toBitFull;
				struct {
					U8 toBit      : 6;
					U8 secondCard : 1;
				};
			};
		};
	};
};

struct Transposition {
	U64 hash;
	Score score;
	Depth depth;
	TranspositionMove move;
};


struct TranspositionEntry {
	Transposition replaceAlways;
	Transposition depthPreferred;
};

constexpr U64 TT_SIZE = (1ULL << 29) / sizeof(Transposition);



class TranspositionTableWrapper {
	std::vector<TranspositionEntry> table;

public:
	TranspositionTableWrapper();

	Transposition get(U64 hash) const;
	void put(Transposition transposition);
};