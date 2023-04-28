#include "board.h"
#include "boardWin.hpp"
#include "boardIter.hpp"
#include "boardEval.hpp"
#include "game.h"



// negamax implementation
// p0 is the maximizer
template<bool player, bool root, bool trackDistance>
std::conditional_t<root, SearchResult, Score> Board::search(Game& game, Score alpha, Score beta, Depth depthLeft) {
	Board beforeBoard = *this;

	U8 thisCardI = cardI;
	const auto& moveList = game.cards->moveBoards[CARDS_HAND[player][cardI]];
	assertValid(*game.cards, player);

	if (isWinInOne<player>(moveList)) {
		Board board = *this;
		if (root) {
			board.doWinInOne<player>(moveList);
			board.assertValid(*game.cards, !player, true);
		}
		return SearchResult{ SCORE::WIN, board, true, true };
	}

	if (!root && depthLeft <= 0) {
		Score standing_pat = evaluate<player>();
		if (standing_pat >= beta)
			return SearchResult{ beta };
		if (standing_pat > alpha)
			alpha = standing_pat;
	}

	if (!root && depthLeft > 0 && !trackDistance) { // TODO: trackdistance..
		auto ttEntry = game.tt.get(hash);
		if (ttEntry.hash) {
			if (ttEntry.depth >= depthLeft || std::abs(ttEntry.score) >= SCORE::WIN) {
				if (ttEntry.type == BoundType::EXACT)
					return SearchResult{ ttEntry.score };
				if (ttEntry.type == BoundType::LOWER) {
					if (ttEntry.score > alpha)
						alpha = ttEntry.score;
				} else { // BoundType::UPPER
					if (ttEntry.score < beta)
						beta = ttEntry.score;
				}
				if (alpha >= beta)
					return SearchResult{ ttEntry.score };
			}
		}
	}

	// TODO: TT bestmove logic

	Score alphaOrig = alpha;
	Board nextBoard;
	bool foundMove = false;
	Score bestScore = SCORE::MIN;
	iterateMoves<player>(game, !root && depthLeft <= 0, [&]() {
		foundMove = true;
		Board beforeBoard = *this;
		Score score = -search<!player, false, trackDistance>(game, -(beta + (beta >= 0 ? trackDistance : -trackDistance)), -(alpha + (alpha >= 0 ? trackDistance : -trackDistance)), depthLeft - 1);
		// trackDistance: widen the search window so we can subtract one again to penalize for distance

		if (trackDistance) // move score closer to zero for every move
			score -= score >= 0 ? 1 : -1;

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

	if (depthLeft > 0 && !trackDistance) // TODO: trackdistance..
		game.tt.put({
			.hash = hash,
			.score = alpha,
			.depth = depthLeft,
			// .fromBit =
			// .secondCard =
			// .toBit =
			.type = alpha <= alphaOrig ? BoundType::UPPER : alpha >= beta ? BoundType::LOWER : BoundType::EXACT,
		});


	assume(*this == beforeBoard);

	return SearchResult{ alpha, nextBoard, foundMove };
}