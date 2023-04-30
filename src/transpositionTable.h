#pragma once

#include "types.h"
#include "board.h"

#include <array>




enum BoundType : U8 {
	NONE = 0,
	LOWER = 1,
	UPPER = 2,
	EXACT = LOWER | UPPER,
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
	Depth depth;
	TranspositionMove move;
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