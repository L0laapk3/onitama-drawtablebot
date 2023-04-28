#include "board.h"
#include "boardSearch.hpp"

#include <chrono>
#include <iostream>



void narrowSearchForWin(ScoreParsed& score, Score& alpha, Score& beta) {
	if (score.outcome == SCORE::WIN) {
		beta = SCORE::WIN;
		alpha = beta - (DEPTH_MAX + 1);
	} else if (score.outcome == SCORE::LOSS) {
		alpha = SCORE::LOSS;
		beta = alpha + (DEPTH_MAX + 1);
	}
}


SearchResult Board::search(Game& game, Depth depth, bool player, bool searchWin, Score alpha, Score beta, bool print) {
	assertValid(*game.cards, player);
	SearchResult result;
	result.durationUs = 0;
	if (!searchWin) {
		auto start = std::chrono::high_resolution_clock::now();
		if (player)
			result = search<1, true>(game, alpha, beta, depth);
		else
			result = search<0, true>(game, alpha, beta, depth);
		auto end = std::chrono::high_resolution_clock::now();
		result.durationUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

		auto parsed = parseScore(result.score);
		if (parsed.outcome != SCORE::DRAW) {
			searchWin = true;
 			narrowSearchForWin(parsed, alpha, beta);
		}
	}

	if (searchWin) {
		// search again and care about win distance
		auto prevDuration = result.durationUs;
		auto start = std::chrono::high_resolution_clock::now();
		if (player)
			result = search<1, true, true>(game, alpha, beta, depth);
		else
			result = search<0, true, true>(game, alpha, beta, depth);
		auto end = std::chrono::high_resolution_clock::now();
		result.durationUs = prevDuration + std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	if (print)
		printf("Depth: %2d, Score: %s, Time: %lldms\n", depth, scoreToString(result.score, player).c_str(), result.durationUs / 1000);

	if (!result.foundMove)
		std::cout << "no move found" << std::endl;

	assertValid(*game.cards, player, result.winningMove);

	return result;
}



SearchTimeResult Board::searchTime(Game& game, S64 timeMs, bool player, bool searchWin, Score alpha, Score beta) {
	SearchResult result;
	ScoreParsed parsedScore{};
	Depth depth = 0;

	S64 lastDurationUs = 1, lastDurationUs2 = 1;
	while (depth < 511) {
		depth++;

		result = search(game, depth, player, (parsedScore.outcome != SCORE::DRAW) || searchWin, alpha, beta, false);
		if (result.winningMove)
			break;
		parsedScore = parseScore(result.score);
		if (parsedScore.outcome != SCORE::DRAW && parsedScore.outcomeDistance <= depth)
			break;

		S64 predictedTime = result.durationUs * std::min((double)result.durationUs / lastDurationUs, (double)lastDurationUs / lastDurationUs2);
		lastDurationUs2 = lastDurationUs;
		lastDurationUs = std::max<S64>(result.durationUs, 1);
		if (predictedTime > timeMs * 1000)
			break;
	}
	printf("Depth: %2d, Score: %s, Time: %lldms\n", depth, scoreToString(result.score, player).c_str(), result.durationUs / 1000);
	return {
		result,
		depth,
	};
}


constexpr Score PANIC_TRESHOLD = MUL_POSITION_ADVANTAGE / 2;
constexpr int PANIC_TIME_MULTIPLIER = 8;