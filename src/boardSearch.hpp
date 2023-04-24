#include "board.h"
#include "boardWin.hpp"
#include "boardIter.hpp"
#include "boardEval.hpp"


// negamax implementation
// p0 is the maximizer
template<bool player, bool root>
std::conditional_t<root, SearchResult, Score> Board::search(const MoveBoardSet& moveBoards, Score alpha, Score beta, Depth depthLeft) {

	U8 thisCardI = cardI;
	const auto& moveList = moveBoards[CARDS_HAND[player][cardI]];

	if (isWinInOne<player>(moveList)) {
		Board board = *this;
		board.doWinInOne<player>(moveList);
		board.assertValid(true);
		return SearchResult{ SCORE::WIN, board };
	}

	if (!root && depthLeft <= 0) {
		Score standing_pat = evaluate<player>();
		if (standing_pat >= beta)
			return SearchResult{ beta, Board{} };
		if (standing_pat > alpha)
			alpha = standing_pat;
	}

	Board nextBoard;
	iterateMoves<player>(moveBoards, depthLeft <= 0, [&]() {
		Score score = -search<!player>(moveBoards, -beta, -alpha, depthLeft - 1);
		if (root && score > alpha)
			nextBoard = *this;
		if (score >= beta) {
			alpha = beta;
			return false;
		}
		if (score > alpha)
			alpha = score;
		return true;
	});

	return SearchResult{ alpha, nextBoard };
}



template<bool player>
SearchResult Board::search(const MoveBoardSet& moveBoards, Depth depth) {
	return search<player, true>(moveBoards, SCORE::LOSE, SCORE::WIN, depth);
}