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
class Board {
public:
    std::array<U32, 2> p;
    std::array<U32, 2> k;
	U8 cardsIndex;

	// boardUtil.cpp
	static Board create(const CardsInfo& cards, std::array<U32, 2> p = { 0b11111'00000'00000'00000'00000, 0b00000'00000'00000'00000'11111 }, std::array<U32, 2> k = { 0b00100'00000'00000'00000'00000, 0b00000'00000'00000'00000'00100 });
	void print() const;
	Board invert() const;

	// boardWin.hpp
	static constexpr std::array<U32, 2> TEMPLE = { 22, 2 };
	static U32 isKingAttackedBy(U32 bbk, U32 bbp);
	template <bool player>
	U32 isKingAttacked(U32 bbk) const;
	template <bool player>
	bool isTempleKingInRange() const;
	template <bool player>
	bool isTempleFree() const;

	template <bool player>
	bool isTempleWinInOne() const;
	template <bool player>
	U32 isTakeWinInOne() const;
	template <bool player>
	bool isWinInOne() const;

	template <bool player>
	void doWinInOne();

	// boardEval.hpp
	template <bool player>
	Score evaluate() const;
	template <bool player>
	Score evaluateHalf() const;

	// boardIter.hpp
	template<bool player>
	void iterateMoves(bool quiesence, const std::function<bool()> f);

	// boardSearch.hpp
	template<bool player, bool root = false>
	std::conditional_t<root, SearchResult, Score> search(Score alpha, Score beta, Depth depthLeft);
	template<bool player>
	SearchResult search(Depth depth);
};

struct SearchResult {
	Score score;
	Board next;
	operator Score() const { return score; };
};