#include "board.h"
#include "boardSearch.hpp"

#include <chrono>
#include <iostream>



void narrowSearchForWin(ScoreParsed& score, Score& alpha, Score& beta) {
	if (score.outcome == SCORE::WIN) {
		beta = SCORE::WIN;
		alpha = SCORE::WIN - (DEPTH_MAX + 1);
	} else if (score.outcome == SCORE::LOSS) {
		alpha = SCORE::LOSS;
		beta = SCORE::LOSS + (DEPTH_MAX + 1);
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


SearchTimeResult Board::searchTime(Game& game, S64 timeMs, Depth maxDepth, bool player, Score lastScore, Depth depth) {
	SearchResult result;
	ScoreParsed parsedScore{}, lastParsed = parseScore(lastScore);

	Score alpha, beta;
	if (lastParsed.outcome == SCORE::DRAW) {
		alpha = lastScore - MUL_PIECE_ADVANTAGE / 10;
		beta  = lastScore + MUL_PIECE_ADVANTAGE / 10;
	} else {
		narrowSearchForWin(lastParsed, alpha, beta);
		depth--; // 1 less depth, we don't need to search the next depth if its a forced loss/win
	}
	// printf("alpha: %s, beta: %s\n", scoreToString(alpha).c_str(), scoreToString(beta).c_str());

	S64 lastDurationUs = 1, lastDurationUs2 = 1;
	bool widenedAspirationWindow = false;
	while (depth < maxDepth) {
		depth++;
		bool searchBoundsFail = true;
		do {
			result = search(game, depth, player, parsedScore.outcome != SCORE::DRAW || lastParsed.outcome != SCORE::DRAW, alpha, beta, false);
			if (result.winningMove)
				goto stopSearch;
			parsedScore = parseScore(result.score);
			if (parsedScore.outcome != SCORE::DRAW && parsedScore.outcomeDistance <= depth + 1)
				goto stopSearch;

			if (result.score <= alpha) {
				if (!widenedAspirationWindow)
					alpha -= MUL_PIECE_ADVANTAGE;
				else
					alpha = SCORE::LOSS;
				widenedAspirationWindow = true;
				// printf("fail low:  alpha: %s, score: %s, beta: %s at depth %2d\n", scoreToString(alpha).c_str(), scoreToString(result.score).c_str(), scoreToString(beta).c_str(), depth);
			} else if (result.score >= beta) {
				if (!widenedAspirationWindow)
					beta += MUL_PIECE_ADVANTAGE;
				else
					beta = SCORE::WIN;
				widenedAspirationWindow = true;
				// printf("fail high: alpha: %s, score: %s, beta: %s at depth %2d\n", scoreToString(alpha).c_str(), scoreToString(result.score).c_str(), scoreToString(beta).c_str(), depth);
			} else
				searchBoundsFail = false;
		} while (searchBoundsFail);

		S64 predictedTime = result.durationUs * std::max(((double)result.durationUs) / lastDurationUs, ((double)lastDurationUs) / lastDurationUs2);
		lastDurationUs2 = lastDurationUs;
		lastDurationUs = std::max<S64>(result.durationUs, 1);
		if (result.durationUs > timeMs * 1000 / 3) // 3 is an arbitrary estimation of the increased branching factor from next iteration depth
			break;
	}
stopSearch:
	printf("Depth: %2d, Score: %s, Time: %lldms\n", depth, scoreToString(result.score, player).c_str(), result.durationUs / 1000);
	return {
		result,
		depth,
	};
}


constexpr Score PANIC_TRESHOLD = MUL_POSITION_ADVANTAGE / 2;
constexpr int PANIC_TIME_MULTIPLIER = 8;