#pragma once

#include "Types.h"

#include <array>



enum Score : int {
	WIN = std::numeric_limits<int>::max(),
	LOSS = -std::numeric_limits<int>::max(),
	DRAW = 0,
};
typedef int Score;
typedef int Depth;



class Board {
public:
    std::array<U32, 2> p;
    std::array<U32, 2> k;
	U8 cardsIndex;

	template<bool player>
	bool isPWin() const;
	template<bool player>
	bool isPWinIn1() const;
	Value evaluate() const;

	Score search(Score alpha, Score beta, Depth remaingDepth);
protected:
	void inverseIterateMovesStart(bool queisence);
	bool inverseIterateMovesNext(bool queisence);
	void inverseIterateMovesEnd(bool queisence);
};