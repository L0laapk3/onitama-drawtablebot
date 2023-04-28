#include "board.h"
#include "boardWin.hpp"
#include "boardIter.hpp"
#include "boardEval.hpp"
#include "game.h"
#include "transpositionTable.hpp"



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

	Transposition* ttWriteEntry;
	if (!root && depthLeft > 0 && !trackDistance) { // TODO: trackdistance..
		auto ttReadEntry = game.tt.get(hash);
		if (ttReadEntry.hash) {
			if (ttReadEntry.depth >= depthLeft || std::abs(ttReadEntry.score) >= SCORE::WIN) {
				if (ttReadEntry.move.type == BoundType::EXACT)
					return SearchResult{ ttReadEntry.score };
				if (ttReadEntry.move.type == BoundType::LOWER) {
					if (ttReadEntry.score > alpha)
						alpha = ttReadEntry.score;
				} else { // BoundType::UPPER
					if (ttReadEntry.score < beta)
						beta = ttReadEntry.score;
				}
				if (alpha >= beta)
					return SearchResult{ ttReadEntry.score };
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

	if (depthLeft > 0 && !trackDistance) { // TODO: trackdistance..
		TranspositionMove move;
		move.type = alpha <= alphaOrig ? BoundType::UPPER : alpha >= beta ? BoundType::LOWER : BoundType::EXACT;
		game.tt.put({
			.hash = hash,
			.score = alpha,
			.depth = depthLeft,
			.move = move,
		});
	}


	assume(*this == beforeBoard);

	return SearchResult{ alpha, nextBoard, foundMove };
}