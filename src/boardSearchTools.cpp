#include "board.h"
#include "boardSearch.hpp"

#include <chrono>
#include <iostream>




SearchResult Board::search(const CardsInfo& cards, Depth depth, bool player, Score alpha, Score beta, bool print) {
	SearchResult result;
	auto start = std::chrono::high_resolution_clock::now();
	if (player)
		result = search<1, true>(cards, alpha, beta, depth);
	else
		result = search<0, true>(cards, alpha, beta, depth);
	auto end = std::chrono::high_resolution_clock::now();
	result.durationUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	if (print)
		printf("Depth: %d, Time: %lldms\n", depth, result.durationUs / 1000);

	return result;
}



SearchTimeResult Board::searchTime(const CardsInfo& cards, S64 timeMs, bool player, Score alpha, Score beta) {
	SearchResult result;
	Depth depth = 0;

	S64 lastDurationUs = 1;
	while (depth < 512) {
		depth++;

		result = search(cards, player, depth, alpha, beta, false);
		if (result.winningMove)
			break;

		int predictedTime = result.durationUs * result.durationUs / std::max<int>(lastDurationUs, 1);
		lastDurationUs = result.durationUs;
		if (predictedTime > timeMs * 1000)
			break;
	}
	printf("Depth: %d, Time: %lldms\n", depth, result.durationUs / 1000);
	return {
		result,
		depth,
	};
}


constexpr Score PANIC_TRESHOLD = MUL_POSITION_ADVANTAGE / 2;
constexpr int PANIC_TIME_MULTIPLIER = 8;