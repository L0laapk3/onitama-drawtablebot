#pragma once

#include "types.h"
#include "score.h"
#include "card.hpp"

#include <array>
#include <functional>
#include <limits>
#include <type_traits>
#include <string>
#include <span>


class Game;
struct SearchResult;
struct SearchTimeResult;
class Board {
public:
    std::array<U32, 2> p;
    std::array<U32, 2> k;
	U8 cardI = 10;
	U64 hash = 0;


	// boardUtil.cpp
	static Board create(bool player = 0, std::array<U32, 2> p = { 0b00000'00000'00000'00000'11111, 0b11111'00000'00000'00000'00000 }, std::array<U32, 2> k = { 0b00000'00000'00000'00000'00100, 0b00100'00000'00000'00000'00000 });
private:
	static std::string toString(const CardsInfo& cards, std::vector<Board> boards, std::vector<char> turnIndicators);
public:
	static std::string toString(const CardsInfo& cards, std::vector<Board> boards, std::vector<bool> players);
	static std::string toString(const CardsInfo& cards, std::vector<Board> boards);
	std::string toString(const CardsInfo& cards, bool player) const;
	std::string toString(const CardsInfo& cards) const;
	void print(const CardsInfo& cards) const;
	void print(const CardsInfo& cards, bool player) const;
	Board invert(bool player) const;
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
	void iterateMoves(const Game& game, bool quiesence, const Callable f);

	// boardSearch.hpp
	template<bool player, bool root = false, bool trackDistance = false>
	std::conditional_t<root, SearchResult, Score> search(const Game& game, Score alpha, Score beta, Depth depthLeft);

	// boardSearchTools.cpp
	SearchResult search(const Game& game, Depth depth, bool player = 0, bool searchWin = false, Score alpha = SCORE::LOSS, Score beta = SCORE::WIN, bool print = true);
	SearchTimeResult searchTime(const Game& game, S64 timeMs, bool player = 0, bool searchWin = false, Score alpha = SCORE::LOSS, Score beta = SCORE::WIN);
	// SearchTimeResult searchTimeWithPanic(const Game& game, S64 timeMs, bool player = 0, SearchTimeResult& lastResult);
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