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

typedef int Depth;

struct SearchResult;
struct SearchTimeResult;
class Board {
public:
    std::array<U32, 2> p;
    std::array<U32, 2> k;
	U8 cardI = 10;


	// boardUtil.cpp
	static Board create(std::array<U32, 2> p = { 0b00000'00000'00000'00000'11111, 0b11111'00000'00000'00000'00000 }, std::array<U32, 2> k = { 0b00000'00000'00000'00000'00100, 0b00100'00000'00000'00000'00000 });
private:
	void print(const CardsInfo& cards, char turnIndicator) const;
public:
	void print(const CardsInfo& cards) const;
	void print(const CardsInfo& cards, bool player) const;
	Board invert() const;
	void checkValid(const CardsInfo& cards, bool isWon = false) const;
	void assertValid(const CardsInfo& cards, bool isWon = false) const;
	bool operator==(const Board& other) const;

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
	template<bool player>
	void iterateMoves(const CardsInfo& cards, bool quiesence, const std::function<bool()> f);

	// boardSearch.hpp
	template<bool player, bool root = false>
	std::conditional_t<root, SearchResult, Score> search(const CardsInfo& cards, Score alpha, Score beta, Depth depthLeft);
	SearchResult search(const CardsInfo& cards, bool player, Depth depth, Score alpha = SCORE::LOSE, Score beta = SCORE::WIN);
	SearchTimeResult searchTime(const CardsInfo& cards, bool player, int timeMs, Score alpha = SCORE::LOSE, Score beta = SCORE::WIN);
	// SearchTimeResult searchTimeWithPanic(const CardsInfo& cards, bool player, int timeMs, SearchTimeResult& lastResult);
};

struct SearchResult {
	Score score;
	Board board;
	bool foundMove = true;
	bool winningMove = false;
	operator Score() const { return score; };
};
struct SearchTimeResult : public SearchResult {
	Depth depth;
	S64 timeMs;
};