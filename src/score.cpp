#include "score.h"

#include <sstream>
#include <iomanip>




ScoreParsed parseScore(Score score) {
	ScoreParsed result{};
	result.eval = score;
	if (score >= SCORE::WIN - DEPTH_MAX) {
		result.outcome = SCORE::WIN;
		result.outcomeDistance = SCORE::WIN - score + 1;
	} else if (score <= SCORE::LOSS + DEPTH_MAX) {
		result.outcome = SCORE::LOSS;
		result.outcomeDistance = score - SCORE::LOSS + 1;
	} else
		result.outcome = SCORE::DRAW;
	return result;
}

std::string scoreToString(Score score, bool player) {
	return scoreToString(parseScore(score), player);
}
std::string scoreToString(ScoreParsed parsed, bool player) {
	if (parsed.outcome != SCORE::DRAW)
		return std::string((parsed.outcome == SCORE::WIN) != player ? " win" : "lose") + " in " + std::to_string(parsed.outcomeDistance);

	if (!player)
		parsed.eval *= -1;

	std::stringstream stream;
	stream << (parsed.eval > 0 ? "+" : parsed.eval == SCORE::DRAW ? " " : "") << std::fixed << std::setprecision(4) << ((double)parsed.eval / MUL_PIECE_ADVANTAGE);
	return stream.str();
}