#include "game.h"

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


SearchResult Game::search(Depth depth, bool searchWin, Score alpha, Score beta, bool print) {
	board.assertValid(*cards, player);
	SearchResult result;
	result.durationUs = 0;

	if (!searchWin) {
		auto start = std::chrono::high_resolution_clock::now();
		if (player)
			(RootResult&)result = board.search<1, true>(*this, alpha, beta, depth);
		else
			(RootResult&)result = board.search<0, true>(*this, alpha, beta, depth);
		auto end = std::chrono::high_resolution_clock::now();
		result.durationUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

		auto parsed = parseScore(result.score);
		if (parsed.outcome != SCORE::DRAW) {
			searchWin = true;
 			// narrowSearchForWin(parsed, alpha, beta);
		}
	}

	if (searchWin) {
		// search again and care about win distance
		auto prevDuration = result.durationUs;
		auto start = std::chrono::high_resolution_clock::now();
		if (player)
			(RootResult&)result = board.search<1, true, true>(*this, alpha, beta, depth);
		else
			(RootResult&)result = board.search<0, true, true>(*this, alpha, beta, depth);
		auto end = std::chrono::high_resolution_clock::now();
		result.durationUs = prevDuration + std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	if (print)
		printf("Depth: %2d, Score: %s, Time: %lldms\n", depth, scoreToString(result.score, player).c_str(), result.durationUs / 1000);

	if (!result.foundMove)
		std::cout << "no move found" << std::endl;

	board.assertValid(*cards, player, result.winningMove);

	return result;
}



SearchTimeResult Game::searchTime(SearchStopCriteria stop, SearchPersistent& persistent) {
	SearchTimeResult result;
	ScoreParsed parsedScore{}, lastParsed = parseScore(persistent.lastScore);

	Depth depth = persistent.lastDepth;
	bool lastIsMate = lastParsed.outcome != SCORE::DRAW;
	if (lastIsMate) {
		depth--; // 1 less depth, we don't need to search the next depth if its a forced loss/win
	}
	// printf("alpha: %s, beta: %s\n", scoreToString(alpha).c_str(), scoreToString(beta).c_str());

	S64 lastDurationUs = 1, lastDurationUs2 = 1;
	bool widenedAspirationWindow = false;
	while (depth < stop.depth) {
		depth++;
		while (true) {
			// printf("window: [%s, %s]\n", scoreToString(persistent.alpha).c_str(), scoreToString(persistent.beta).c_str());
			(SearchResult&)result = search(depth, parsedScore.outcome != SCORE::DRAW || lastParsed.outcome != SCORE::DRAW, persistent.alpha, persistent.beta, false);
			if (result.winningMove)
				goto stopSearch;
			parsedScore = parseScore(result.score);
			if (parsedScore.outcome != SCORE::DRAW) {
				if (!lastIsMate) {
					// narrowSearchForWin(parsedScore, persistent.alpha, persistent.beta);
					lastIsMate = true;
				}
				if (parsedScore.outcomeDistance <= depth + 1)
					goto stopSearch;
			}

			if (result.score <= persistent.alpha) {
				if (!widenedAspirationWindow && persistent.alpha > -MUL_PIECE_ADVANTAGE)
					persistent.alpha -= MUL_PIECE_ADVANTAGE;
				else
					persistent.alpha = SCORE::LOSS;
				widenedAspirationWindow = true;
			} else if (result.score >= persistent.beta) {
				if (!widenedAspirationWindow && persistent.beta < MUL_PIECE_ADVANTAGE)
					persistent.beta += MUL_PIECE_ADVANTAGE;
				else
					persistent.beta = SCORE::WIN;
				widenedAspirationWindow = true;
			} else
				break;
			// printf("new window: [%s, %s] %s\n", scoreToString(persistent.alpha).c_str(), scoreToString(persistent.beta).c_str(), scoreToString(result.score).c_str());
			tt.markRecalculate();
		}

		S64 predictedTime = result.durationUs * std::max(((double)result.durationUs) / lastDurationUs, ((double)lastDurationUs) / lastDurationUs2);
		lastDurationUs2 = lastDurationUs;
		lastDurationUs = std::max<S64>(result.durationUs, 1);
		if (result.durationUs > stop.time * 1000 / 2) // 2 is an arbitrary estimation of the increased branching factor from next iteration depth
			break;
	}
stopSearch:
	printf("Depth: %2d, Score: %s, Time: %lldms\n", depth, scoreToString(result.score).c_str(), result.durationUs / 1000);
	return {
		result,
		depth,
	};
}


SearchTimeResult Game::searchTime(SearchStopCriteria stop) {
	SearchPersistent persistent;
	return searchTime(stop, persistent);
}


// constexpr Score PANIC_TRESHOLD = MUL_POSITION_ADVANTAGE / 2;
// constexpr int PANIC_TIME_MULTIPLIER = 8;