#include "board.h"
#include "boardWin.hpp"
#include "boardIter.hpp"
#include "boardEval.hpp"


// negamax implementation
// p0 is the maximizer
template<bool player, bool root>
std::conditional_t<root, SearchResult, Score> Board::search(const CardsInfo& cards, Score alpha, Score beta, Depth depthLeft) {
	auto permutation = CARDS_PERMUTATIONS[cardI];
	const std::array<const U32*, 2> reverseMoveBoards{
		&(cards.moveBoards[permutation.playerCards[player][0]][!player])[0],
		&(cards.moveBoards[permutation.playerCards[player][1]][!player])[0],
	};

	if (isWinInOne<player>(reverseMoveBoards)) {
		Board board = *this;
		board.doWinInOne<player>(reverseMoveBoards);
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
	iterateMoves<player>(cards, depthLeft <= 0, [&]() {
		Score score = -search<!player>(cards, -beta, -alpha, depthLeft - 1);
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
SearchResult Board::search(const CardsInfo& cards, Depth depth) {
	return search<player, true>(cards, SCORE::LOSE, SCORE::WIN, depth);
}