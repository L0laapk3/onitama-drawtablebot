#include "board.h"
#include "boardWin.hpp"
#include "boardIter.hpp"
#include "boardEval.hpp"
#include "game.h"
#include "transpositionTable.hpp"



// negamax implementation
// p0 is the maximizer
template<bool player, bool root, bool trackDistance, bool quiescence>
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

	if (!root && quiescence) {
		Score standing_pat = evaluate<player>();
		if (standing_pat >= beta)
			return SearchResult{ beta };
		if (standing_pat > alpha)
			alpha = standing_pat;
	}

	TranspositionMove ttBestMove{};
	if (!quiescence) {
		const Transposition* ttReadEntry;
		if (game.tt.get(hash, ttReadEntry)) {
			if (!root && !trackDistance && (ttReadEntry->depth >= depthLeft || std::abs(ttReadEntry->score) >= SCORE::WIN)) { // todo: trackDistance
				if (ttReadEntry->move.type == BoundType::EXACT)
					return SearchResult{ ttReadEntry->score };
				if (ttReadEntry->move.type == BoundType::LOWER) {
					if (ttReadEntry->score > alpha)
						alpha = ttReadEntry->score;
				} else { // BoundType::UPPER
					if (ttReadEntry->score < beta)
						beta = ttReadEntry->score;
				}
				if (alpha >= beta)
					return SearchResult{ ttReadEntry->score };
			}
			ttBestMove = ttReadEntry->move;
			ttBestMove.fromBitFull = ttBestMove.fromBit; // clear boundType
		}
	}

	// TODO: TT bestmove logic
	TranspositionMove bestMove{};
	Score alphaOrig = alpha;
	Board nextBoard;
	bool foundMove = false;
	Score bestScore = SCORE::MIN;
	iterateMoves<player, !root && quiescence>(game, ttBestMove, [&](TranspositionMove& move) {
		foundMove = true;
		Board beforeBoard = *this;
		Score score;
		if (quiescence || depthLeft - 1 <= 0)
			score = -search<!player, false, trackDistance, true>(game, -(beta + (beta >= 0 ? trackDistance : -trackDistance)), -(alpha + (alpha >= 0 ? trackDistance : -trackDistance)), 0);
		else
			score = -search<!player, false, trackDistance, false>(game, -(beta + (beta >= 0 ? trackDistance : -trackDistance)), -(alpha + (alpha >= 0 ? trackDistance : -trackDistance)), depthLeft - 1);
		// trackDistance: widen the search window so we can subtract one again to penalize for distance

		if (trackDistance) // move score closer to zero for every move
			score -= score >= 0 ? 1 : -1;

		if (root && score > bestScore) {
			nextBoard = *this;
			bestScore = score;
		}
		if (score >= beta) {
			alpha = beta;
			if (!root && !trackDistance)
				bestMove = move;
			return false;
		}

		if (score > alpha) {
			alpha = score;
			if (!root && !trackDistance)
				bestMove = move;
		}
		return true;
	});

	if (!quiescence && !root && !trackDistance) { // TODO: trackdistance..
		bestMove.type = alpha <= alphaOrig ? BoundType::UPPER : alpha >= beta ? BoundType::LOWER : BoundType::EXACT;
		game.tt.put({
			.depth = depthLeft,
			.move  = bestMove,
			.score = alpha,
			.hash  = hash,
		});
	}


	assume(*this == beforeBoard);

	return SearchResult{ alpha, nextBoard, foundMove };
}