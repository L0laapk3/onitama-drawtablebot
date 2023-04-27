#include "score.h"

#include <sstream>
#include <iomanip>




ScoreParsed parseScore(Score score) {
	ScoreParsed result{};
	result.eval = score;
	if (score >= SCORE::WIN - DEPTH_MAX) {
		result.outcome = SCORE::WIN;
		result.outcomeDistance = SCORE::WIN - score + 1;
	} else if (score <= SCORE::LOSE + DEPTH_MAX) {
		result.outcome = SCORE::LOSE;
		result.outcomeDistance = score - SCORE::LOSE + 1;
	} else {
		result.outcome = SCORE::DRAW;
		result.outcomeDistance = 0;
	}
	return result;
}

std::string scoreToString(Score score) {
	return scoreToString(parseScore(score));
}
std::string scoreToString(ScoreParsed parsed) {
	if (parsed.outcome != SCORE::DRAW)
		return std::string(parsed.outcome == SCORE::WIN ? "win" : "lose") + " in " + std::to_string(parsed.outcomeDistance);

	std::stringstream stream;
	stream << (parsed.eval > 0 ? "+" : parsed.eval == SCORE::DRAW ? " " : "") << std::fixed << std::setprecision(4) << ((double)parsed.eval / MUL_PIECE_ADVANTAGE);
	return stream.str();
}