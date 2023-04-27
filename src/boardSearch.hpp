#include "board.h"
#include "boardWin.hpp"
#include "boardIter.hpp"
#include "boardEval.hpp"



// negamax implementation
// p0 is the maximizer
template<bool player, bool root, bool trackDistance>
std::conditional_t<root, SearchResult, Score> Board::search(const CardsInfo& cards, Score alpha, Score beta, Depth depthLeft) {
	Board beforeBoard = *this;

	U8 thisCardI = cardI;
	const auto& moveList = cards.moveBoards[CARDS_HAND[player][cardI]];
	assertValid(cards, player);

	if (isWinInOne<player>(moveList)) {
		Board board = *this;
		if (root) {
			board.doWinInOne<player>(moveList);
			board.assertValid(cards, player, true);
		}
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
	bool foundMove = false;
	Score bestScore = SCORE::MIN;
	iterateMoves<player>(cards, !root && depthLeft <= 0, [&]() {
		foundMove = true;
		Board beforeBoard = *this;
		Score score = -search<!player>(cards, -beta, -alpha, depthLeft - 1);

		if (trackDistance)
			score -= score >= 0 ? 1 : -1; // move score closer to zero for every move

		if (root && score > bestScore) {
			nextBoard = *this;
			bestScore = score;
		}
		if (score >= beta) {
			alpha = beta;
			return false;
		}
		if (score > alpha)
			alpha = score;
		return true;
	});

	assume(*this == beforeBoard);

	return SearchResult{ alpha, nextBoard, foundMove };
}