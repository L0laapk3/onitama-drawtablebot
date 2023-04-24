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
	U8 cardI;


	// boardUtil.cpp
	static Board create(const CardsInfo& cards, std::array<U32, 2> p = { 0b11111'00000'00000'00000'00000, 0b00000'00000'00000'00000'11111 }, std::array<U32, 2> k = { 0b00100'00000'00000'00000'00000, 0b00000'00000'00000'00000'00100 });
	void print() const;
	Board invert() const;
	void checkValid(bool isWon = false) const;
	void assertValid(bool isWon = false) const;
	bool operator==(const Board& other) const;

	// boardWin.hpp
	static constexpr std::array<U32, 2> TEMPLE = { 2, 22 };
	static U32 isKingAttackedBy(const std::array<const U32*, 2>& reverseMoveBoards, U32 bbk, U32 bbp);
	template <bool player>
	U32 isKingAttacked(const std::array<const U32*, 2>& reverseMoveBoards, U32 bbk) const;
	template <bool player>
	bool isTempleKingInRange(const std::array<const U32*, 2>& reverseMoveBoards) const;
	template <bool player>
	bool isTempleFree() const;

	template <bool player>
	bool isTempleWinInOne(const std::array<const U32*, 2>& reverseMoveBoards) const;
	template <bool player>
	U32 isTakeWinInOne(const std::array<const U32*, 2>& reverseMoveBoards) const;
	template <bool player>
	bool isWinInOne(const std::array<const U32*, 2>& reverseMoveBoards) const;

	template <bool player>
	void doWinInOne(const std::array<const U32*, 2>& reverseMoveBoards);

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
	template<bool player>
	SearchResult search(const CardsInfo& cards, Depth depth);
};

struct SearchResult {
	Score score;
	Board next;
	operator Score() const { return score; };
};