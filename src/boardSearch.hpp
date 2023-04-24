#include "board.h"
#include "boardWin.hpp"
#include "boardIter.hpp"
#include "boardEval.hpp"

#include <chrono>


// negamax implementation
// p0 is the maximizer
template<bool player, bool root>
std::conditional_t<root, SearchResult, Score> Board::search(const CardsInfo& cards, Score alpha, Score beta, Depth depthLeft) {

	U8 thisCardI = cardI;
	const auto& moveList = cards.moveBoards[CARDS_HAND[player][cardI]];
	assertValid(cards);

	if (isWinInOne<player>(moveList)) {
		Board board = *this;
		board.doWinInOne<player>(moveList);
		board.assertValid(cards, true);
		return SearchResult{ SCORE::WIN, board, true, true };
	}

	if (!root && depthLeft <= 0) {
		Score standing_pat = evaluate<player>();
		if (standing_pat >= beta)
			return SearchResult{ beta, Board{} };
		if (standing_pat > alpha)
			alpha = standing_pat;
	}

	Board nextBoard;
	nextBoard.p[0] = 0;
	iterateMoves<player>(cards, depthLeft <= 0, [&]() {
		Board beforeBoard = *this;
		Score score = -search<!player>(cards, -beta, -alpha, depthLeft - 1);

		score -= score >= 0 ? 1 : -1; // move score closer to zero for every move

		if (score > alpha)
			nextBoard = *this;
		if (score >= beta) {
			alpha = beta;
			return false;
		}
		if (score > alpha)
			alpha = score;
		return true;
	});

	return SearchResult{ alpha, nextBoard, nextBoard.p[0] != 0 };
}





SearchResult Board::search(const CardsInfo& cards, bool player, Depth depth, Score alpha, Score beta) {
	if (player)
		return search<1, true>(cards, alpha, beta, depth);
	return     search<0, true>(cards, alpha, beta, depth);
}



SearchTimeResult Board::searchTime(const CardsInfo& cards, bool player, int timeMs, Score alpha, Score beta) {
	SearchResult result;
	Depth depth = 0;

	S64 duration, lastDuration = 1;
	auto start = std::chrono::high_resolution_clock::now();
	while (true) {
		depth++;

		result = search(cards, player, depth, alpha, beta);
		auto end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (result.winningMove)
			break;

		int predictedTime = duration * duration / std::max<int>(lastDuration, 1);
		lastDuration = duration;
		if (predictedTime > timeMs)
			break;
	}
	printf("Depth: %d, Time: %lldms\n", depth, duration);
	return {
		result,
		depth,
		duration,
	};
}


constexpr Score PANIC_TRESHOLD = MUL_POSITION_ADVANTAGE / 2;
constexpr int PANIC_TIME_MULTIPLIER = 8;