#include "board.h"

#include "card.hpp"

#include <immintrin.h>
#include <x86intrin.h>



constexpr Score MUL_POSITION_ADVANTAGE = 1024;
constexpr Score MUL_PIECE_ADVANTAGE    = 1024 * MUL_POSITION_ADVANTAGE;

template <bool player>
Score inline Board::evaluateHalf() const {
	Score score = 0;

	score += MUL_PIECE_ADVANTAGE * _popcnt32(p[player]);

	return score;
}

template <bool player>
Score inline Board::evaluate() const {

	return evaluateHalf<player>() - evaluateHalf<!player>();
}