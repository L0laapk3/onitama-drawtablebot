#include "board.h"
#include "boardWin.hpp"
#include "boardIter.hpp"
#include "boardEval.hpp"
#include "game.h"
#include "transpositionTable.hpp"



// negamax implementation
// p0 is the maximizer
template<bool player, bool root, bool trackDistance, bool quiescence>
std::conditional_t<root, RootResult, Score> Board::search(Game& game, std::vector<KillerMoves>::reverse_iterator& killerMoves, Score alpha, Score beta, Depth depthLeft) {
	Board beforeBoard = *this;
	Score alphaOrig = alpha;

	U8 thisCardI = cardI;
	const auto& moveList = game.cards->moveBoards[CARDS_HAND[player][cardI]];
	assertValid(*game.cards, player);

	if (isWinInOne<player>(moveList)) {
		Board board = *this;
		if (root) {
			board.doWinInOne<player>(moveList);
			board.assertValid(*game.cards, !player, true);
		}
		return RootResult{ SCORE::WIN, board, true, true };
	}

	if constexpr (!root && quiescence) {
		Score standing_pat = evaluate<player>();
		if (standing_pat >= beta)
			return beta;
		if (standing_pat > alpha)
			alpha = standing_pat;
	}

	Move ttBestMove{};
	if constexpr (!quiescence) {
		const Transposition* ttReadEntry;
		if (game.tt.get(hash, ttReadEntry)) {
			if constexpr (!root) {
				// TT cutoff copied from stockfish - works much better than wikipedia/chessprogramming's version
				if (ttReadEntry->depth >= depthLeft || std::abs(ttReadEntry->score) >= SCORE::WIN - (trackDistance ? ttReadEntry->depth : 0)) {
				// if (ttReadEntry->depth > depthLeft - (ttReadEntry->move.bound == Bound::EXACT) || std::abs(ttReadEntry->score) >= SCORE::WIN)
					// if (ttReadEntry->move.bound & (ttReadEntry->score >= beta ? Bound::LOWER : Bound::UPPER))
					// 	return ttReadEntry->score;

					if (ttReadEntry->move.bound & Bound::UPPER && ttReadEntry->score < beta)
						beta = ttReadEntry->score;
					if (ttReadEntry->move.bound & Bound::LOWER && ttReadEntry->score > alpha)
						alpha = ttReadEntry->score;
					if (alpha >= beta)
						return ttReadEntry->score;
				}
			}
			ttBestMove = ttReadEntry->move;
			ttBestMove.fromBitFull = ttBestMove.fromBit; // clear bound
		}
	}

	// TODO: TT bestmove logic
	Move bestMove{};
	Board nextBoard;
	bool foundMove = false;
	Score bestRootScore = SCORE::MIN;
	iterateMoves<player, !root && quiescence>(game, ttBestMove, *killerMoves, [&](Move& move) {
		foundMove = true;
		Board beforeBoard = *this;
		Score score;
		if (quiescence || depthLeft - 1 <= 1) // trackDistance: widen the search window so we can subtract one again to penalize for distance
			score = -search<!player, false, trackDistance, true >(game, killerMoves, -(beta + (beta >= 0 ? trackDistance : -trackDistance)), -(alpha + (alpha >= 0 ? trackDistance : -trackDistance)), 1);
		else
			score = -search<!player, false, trackDistance, false>(game, killerMoves, -(beta + (beta >= 0 ? trackDistance : -trackDistance)), -(alpha + (alpha >= 0 ? trackDistance : -trackDistance)), depthLeft - 1);

		if (trackDistance && score != 0) // move score closer to zero for every move
			score -= score >= 0 ? 1 : -1;

		if (root && score > bestRootScore) {
			nextBoard = *this;
			bestRootScore = score;
		}

		if (score > alpha) {
			alpha = score;
			bestMove = move;
			if (alpha >= beta) {
				return false;
			}
		}
		return true;
	});

	if (!quiescence && !root) {
		bestMove.bound = alpha <= alphaOrig ? Bound::UPPER : alpha >= beta ? Bound::LOWER : Bound::EXACT;
		game.tt.put({
			.depth = depthLeft,
			.move  = bestMove,
			.score = alpha,
			.hash  = hash,
		});
	}


	assume(*this == beforeBoard);

	return RootResult{ alpha, nextBoard, foundMove };
}