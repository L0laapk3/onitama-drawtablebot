#pragma once

#include "types.h"

#include <limits>
#include <string>



typedef S16 Depth;

typedef int Score;
enum SCORE : int {
	WIN = std::numeric_limits<int>::max(),
	LOSE = -std::numeric_limits<int>::max(),
	DRAW = 0,
};

constexpr Depth DEPTH_MAX = 512;

constexpr Score MUL_POSITION_ADVANTAGE = 1024;
constexpr Score MUL_PIECE_ADVANTAGE    = 1024 * MUL_POSITION_ADVANTAGE;

struct ScoreParsed {
	Score outcome; // win, lose, 0
	Depth outcomeDistance;
	Score eval; // score of the position
};
ScoreParsed parseScore(Score score);
std::string scoreToString(Score score);
std::string scoreToString(ScoreParsed score);