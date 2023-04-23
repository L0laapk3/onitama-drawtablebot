#include "board.h"



// negamax implementation
// assume p0 is the current player/maximizer
Score Board::search(Score alpha, Score beta, Depth remaingDepth) {
	if (remaingDepth <= 0) {
		Score standing_pat = evaluate();
		if (standing_pat >= beta)
			return beta;
		if (standing_pat > alpha)
			alpha = standing_pat;
	}

	if (isPWin<0>())
		return Score::WIN;

	// if (isPWinIn1<1>())
	// 	return Score::LOSS;

	inverseIterateMovesStart(remaingDepth <= 0);
	while (inverseIterateMovesNext(remaingDepth <= 0)) {
		Score score = -search(-beta, -alpha, remaingDepth - 1);
		if (score > alpha)
			alpha = score;
		if (alpha >= beta)
			break;
	}
	inverseIterateMovesEnd(remaingDepth <= 0);

	return alpha;
}