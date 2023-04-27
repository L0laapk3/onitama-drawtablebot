#include "board.h"

#include "card.hpp"
#include "score.h"

#include <immintrin.h>
#include <x86intrin.h>



constexpr std::array<Score, 33> pawnScores{
	0, 1, 2, 1, 0,
	1, 2, 3, 2, 1,
	2, 3, 4, 3, 2,
	1, 2, 3, 2, 1,
	0, 1, 2, 1, 0,
};
constexpr std::array<Score, 25> kingScores{
	0, 1, 2, 1, 0,
	1, 2, 3, 2, 1,
	2, 3, 4, 3, 2,
	1, 2, 3, 2, 1,
	0, 1, 2, 1, 0,
};
constexpr auto kingScoresMinusPawn = [](){
	std::array<std::array<Score, 25>, 2> scores{};
	for (int flip = 0; flip < 2; flip++)
		for (U64 i = 0; i < 25; i++)
			scores[flip][flip ? 24 - i : i] = kingScores[i] - pawnScores[i];
	return scores;
}();


template <bool player>
Score inline Board::evaluateHalf() const {
	Score score = 0;

	// positional advantage
	const auto getOnePiece = [](U32 bb) {
		U32 piece = bb & -bb;
		return _tzcnt_u32(piece);
	};
	score += MUL_POSITION_ADVANTAGE * kingScoresMinusPawn[player][getOnePiece(k[player])];

	U32 pawns = p[player];
	#pragma unroll
	for (U64 i = 0; i < 5; i++) {
		score += MUL_POSITION_ADVANTAGE * pawnScores[getOnePiece(pawns)];
		pawns &= pawns - 1;
	}

	// piece advantage
	score += MUL_PIECE_ADVANTAGE * _popcnt32(p[player]);


	return score;
}

template <bool player>
Score inline Board::evaluate() const {
	return evaluateHalf<player>() - evaluateHalf<!player>();
}