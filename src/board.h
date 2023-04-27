#pragma once

#include "types.h"
#include "card.hpp"

#include <array>
#include <functional>
#include <limits>
#include <type_traits>


typedef int Score;
enum SCORE : int {
	WIN = std::numeric_limits<int>::max(),
	LOSE = -std::numeric_limits<int>::max(),
	DRAW = 0,
};

typedef S16 Depth;

struct SearchResult;
struct SearchTimeResult;
class Board {
public:
    std::array<U32, 2> p;
    std::array<U32, 2> k;
	U8 cardI = 10;
	U64 hash = 0;


	// boardUtil.cpp
	static Board create(std::array<U32, 2> p = { 0b00000'00000'00000'00000'11111, 0b11111'00000'00000'00000'00000 }, std::array<U32, 2> k = { 0b00000'00000'00000'00000'00100, 0b00100'00000'00000'00000'00000 });
private:
	void print(const CardsInfo& cards, char turnIndicator) const;
public:
	void print(const CardsInfo& cards) const;
	void print(const CardsInfo& cards, bool player) const;
	Board invert() const;
	void checkValid(const CardsInfo& cards, bool player, bool isWon = false) const;
	void assertValid(const CardsInfo& cards, bool player, bool isWon = false) const;
	bool operator==(const Board& other) const;
	void recalculateHash(bool player);

	// boardWin.hpp
	static constexpr std::array<U32, 2> TEMPLE = { 22, 2 };

	template <bool player>
	static U32 isKingAttackedBy(const MoveBoardList& moveList, U32 bbk, U32 bbp);
	template <bool player>
	U32 isKingAttacked(const MoveBoardList& moveList, U32 bbk) const;
	template <bool player>
	bool isTempleKingInRange(const MoveBoardList& moveList) const;
	template <bool player>
	bool isTempleFree() const;

	template <bool player>
	bool isTempleWinInOne(const MoveBoardList& moveList) const;
	template <bool player>
	U32 isTakeWinInOne(const MoveBoardList& moveList) const;
	template <bool player>
	bool isWinInOne(const MoveBoardList& moveList) const;

	template <bool player>
	void doWinInOne(const MoveBoardList& moveList);

	// boardEval.hpp
	template <bool player>
	Score evaluate() const;
	template <bool player>
	Score evaluateHalf() const;

	// boardIter.hpp
	template<bool player, typename Callable>
	void iterateMoves(const CardsInfo& cards, bool quiesence, const Callable f);

	// boardSearch.hpp
	template<bool player, bool root = false>
	std::conditional_t<root, SearchResult, Score> search(const CardsInfo& cards, Score alpha, Score beta, Depth depthLeft);

	// boardSearchTools.cpp
	SearchResult search(const CardsInfo& cards, Depth depth, bool player = 0, Score alpha = SCORE::LOSE, Score beta = SCORE::WIN, bool print = true);
	SearchTimeResult searchTime(const CardsInfo& cards, S64 timeMs, bool player = 0, Score alpha = SCORE::LOSE, Score beta = SCORE::WIN);
	// SearchTimeResult searchTimeWithPanic(const CardsInfo& cards, S64 timeMs, bool player = 0, SearchTimeResult& lastResult);
};

struct SearchResult {
	Score score;
	Board board;
	bool foundMove = true;
	bool winningMove = false;
	operator Score() const { return score; };
	S64 durationUs;
};
struct SearchTimeResult : public SearchResult {
	Depth depth;
};