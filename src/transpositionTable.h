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
		struct {
			union {
				struct {
					U8 fromBit     : 6;
					BoundType type : 2;
				};
				U8 fromBitFull;
			};
			union {
				struct {
					U8 toBit      : 6;
					U8 secondCard : 1;
				};
				U8 toBitFull;
			};
		};
		U16 full;
	};
};

struct Transposition {
	U64 hash;
	Score score;
	Depth depth;
	TranspositionMove move;
};



constexpr U64 TT_SIZE_REPLACE_ALWAYS  = (1ULL << 20) / sizeof(Transposition); // the same for now
constexpr U64 TT_SIZE_DEPTH_PREFERRED = (1ULL << 20) / sizeof(Transposition);



class TranspositionTableWrapper {
	std::vector<Transposition> TableReplaceAlways;
	std::vector<Transposition> TableDepthPreferred;

public:
	TranspositionTableWrapper();

	Transposition get(U64 hash) const;
	void put(Transposition transposition);
};