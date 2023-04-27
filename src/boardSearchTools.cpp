#include "board.h"
#include "boardSearch.hpp"

#include <chrono>
#include <iostream>




SearchResult Board::search(const CardsInfo& cards, Depth depth, bool player, Score alpha, Score beta, bool print) {
	assertValid(cards, player);
	SearchResult result;
	auto start = std::chrono::high_resolution_clock::now();
	if (player)
		result = search<1, true>(cards, alpha, beta, depth);
	else
		result = search<0, true>(cards, alpha, beta, depth);
	auto end = std::chrono::high_resolution_clock::now();
	result.durationUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	if (print)
		printf("Depth: %d, Score: %s, Time: %lldms\n", depth, scoreToString(result.score).c_str(), result.durationUs / 1000);

	if (!result.foundMove)
		std::cout << "no move found" << std::endl;

	assertValid(cards, player, result.winningMove);

	return result;
}



SearchTimeResult Board::searchTime(const CardsInfo& cards, S64 timeMs, bool player, Score alpha, Score beta) {
	SearchResult result;
	Depth depth = 0;

	S64 lastDurationUs = 1;
	while (depth < 3) {
		depth++;

		result = search(cards, depth, player, alpha, beta, false);
		if (result.winningMove)
			break;
		auto parsedScore = parseScore(result.score);
		// if (parsedScore.outcome != SCORE::DRAW && parsedScore.outcomeDistance <= depth)
		// 	break;

		int predictedTime = result.durationUs * result.durationUs / std::max<int>(lastDurationUs, 1);
		lastDurationUs = result.durationUs;
		if (predictedTime > timeMs * 1000)
			break;
	}
	printf("Depth: %d, Score: %s, Time: %lldms\n", depth, scoreToString(result.score).c_str(), result.durationUs / 1000);
	return {
		result,
		depth,
	};
}


constexpr Score PANIC_TRESHOLD = MUL_POSITION_ADVANTAGE / 2;
constexpr int PANIC_TIME_MULTIPLIER = 8;