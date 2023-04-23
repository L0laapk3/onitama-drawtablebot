#include "board.h"

#include "card.hpp"

#include <immintrin.h>
#include <x86intrin.h>



constexpr Score MUL_POSITION_ADVANTAGE = 1000;
constexpr Score MUL_PIECE_ADVANTAGE    = 1000 * MUL_POSITION_ADVANTAGE;

constexpr std::array<Score, 33> pawnScores{
	0, 1, 2, 1, 0,
	2, 4, 7, 4, 2,
	4, 7, 11, 7, 4,
	2, 4, 7, 4, 2,
	0, 1, 2, 1, 0,
};
constexpr std::array<Score, 33> kingScores{
	4, 8, 8, 8, 4,
	3, 5, 8, 5, 3,
	2, 3, 5, 3, 2,
	0, 1, 2, 1, 0,
	0, 0, 1, 0, 0,
};
constexpr auto kingScoresMinusPawn = [](){
	std::array<Score, 33> scores{};
	for (U64 i = 0; i < 33; i++)
		scores[i] = kingScores[i] - pawnScores[i];
	return scores;
}();


template <bool player>
Score inline Board::evaluateHalf() const {
	Score score = 0;

	// positional advantage
	const auto getOnePiece = [](U32 bb) {
		U32 piece = bb & -bb;
		return !player ? _tzcnt_u32(piece) : _lzcnt_u32(piece) - 31 + 24;
	};
	score += MUL_POSITION_ADVANTAGE * kingScoresMinusPawn[getOnePiece(k[player])];

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