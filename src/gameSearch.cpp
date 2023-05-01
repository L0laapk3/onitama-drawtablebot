#include "game.h"

#include "board.h"
#include "boardSearch.hpp"

#include <chrono>
#include <iostream>



void narrowSearchForWin(ScoreParsed& score, Score& alpha, Score& beta) {
	if (score.outcome == SCORE::WIN) {
		beta = SCORE::WIN;
		alpha = SCORE::WIN - DEPTH_MAX;
	} else if (score.outcome == SCORE::LOSS) {
		alpha = SCORE::LOSS;
		beta = SCORE::LOSS + DEPTH_MAX;
	}
}


SearchResult Game::search(Depth depth, bool searchWin, Score alpha, Score beta, bool print) {
	board.assertValid(*cards, player);
	SearchResult result;
	auto start = std::chrono::high_resolution_clock::now();
	if (!searchWin) {
		if (player)
			(RootResult&)result = board.search<1, true>(*this, alpha, beta, depth + 1);
		else
			(RootResult&)result = board.search<0, true>(*this, alpha, beta, depth + 1);
	} else {
		if (player)
			(RootResult&)result = board.search<1, true, true>(*this, alpha, beta, depth + 1);
		else
			(RootResult&)result = board.search<0, true, true>(*this, alpha, beta, depth + 1);
	}
	auto end = std::chrono::high_resolution_clock::now();
	result.durationUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

	if (print)
		printf("Depth: %2d, Score: %s, Time: %lldms\n", depth, scoreToString(result.score, player).c_str(), result.durationUs / 1000);
	// printf("interm Depth: %2d, Score: %s, Time: %lldms, in: [%s, %s], dtm: %d\n", depth, scoreToString(result.score, player).c_str(), result.durationUs / 1000, scoreToString(player ? beta : alpha, player).c_str(), scoreToString(player ? alpha : beta, player).c_str(), searchWin);

	if (!result.foundMove)
		std::cout << "no move found" << std::endl;

	board.assertValid(*cards, player, result.winningMove);

	return result;
}



SearchTimeResult Game::searchTime(SearchStopCriteria stop, SearchPersistent& persistent) {
	SearchTimeResult result;
	ScoreParsed parsedScore{}, lastParsed = parseScore(persistent.lastScore);

	bool searchWin = lastParsed.outcome != SCORE::DRAW;
	// printf("alpha: %s, beta: %s\n", scoreToString(alpha).c_str(), scoreToString(beta).c_str());

	S64 lastDurationUs = 1, lastDurationUs2 = 1;
	bool widenedAspirationWindow = false;
	Depth depth = persistent.lastDepth;
	S64 usedTime = 0;
	stop.time *= 1000;
	while (depth < stop.depth) {
		depth++;
		while (true) {
			while (true) {
				// printf("window: [%s, %s], win: %d\n", scoreToString(persistent.alpha).c_str(), scoreToString(persistent.beta).c_str(), searchWin);
				(SearchResult&)result = search(depth, searchWin, persistent.alpha, persistent.beta, false);
				usedTime += result.durationUs;
				parsedScore = parseScore(result.score);
				if ((parsedScore.outcome != SCORE::DRAW) != searchWin) {
					// switch from/to mate search mode
					searchWin = parsedScore.outcome != SCORE::DRAW;
					// std::cout << "depth: " << depth << " switching search for win: " << searchWin << " score: " << scoreToString(result.score, player) << std::endl;
					tt.markRecalculateScore(searchWin);
					continue;
				}
				if (result.winningMove)
					goto stopSearch;
				// if (parsedScore.outcome != SCORE::DRAW)
				// 	std::cout << "depth " << depth << ": found mate in " << parsedScore.outcomeDistance << std::endl;
				if (parsedScore.outcome != SCORE::DRAW && parsedScore.outcomeDistance <= depth + 1) {
					persistent.lastDepth = parsedScore.outcomeDistance - 2; // limit next search depth to mate distance
					// std::cout << "stop at " << persistent.lastDepth << std::endl;
					goto stopSearchNoDepthSet;
				}
				break;
			}

			if (result.score <= persistent.alpha) {
				if (!widenedAspirationWindow && persistent.alpha > -MUL_PIECE_ADVANTAGE)
					persistent.alpha -= MUL_PIECE_ADVANTAGE;
				else {
					persistent.alpha = SCORE::LOSS;
				}
				widenedAspirationWindow = true;
			} else if (result.score >= persistent.beta) {
				if (!widenedAspirationWindow && persistent.beta < MUL_PIECE_ADVANTAGE)
					persistent.beta += MUL_PIECE_ADVANTAGE;
				else {
					persistent.beta = SCORE::WIN;
				}
				widenedAspirationWindow = true;
			} else
				break;
			// printf("new window: [%s, %s] %s\n", scoreToString(persistent.alpha).c_str(), scoreToString(persistent.beta).c_str(), scoreToString(result.score).c_str());
			tt.markRecalculate();
		}

		S64 predictedTime = result.durationUs * std::max(((double)result.durationUs) / lastDurationUs, ((double)lastDurationUs) / lastDurationUs2);
		lastDurationUs2 = lastDurationUs;
		lastDurationUs = std::max<S64>(result.durationUs, 1);
		if (result.durationUs > stop.time - usedTime)
			break;
	}
stopSearch:
	persistent.lastDepth = depth;
stopSearchNoDepthSet:
	persistent.lastDepth--; // next search: one less depth
	printf("Depth: %2d, Score: %s, Time: %lldms\n", depth, scoreToString(result.score, player).c_str(), usedTime / 1000);
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