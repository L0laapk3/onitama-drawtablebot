#pragma once

#include "board.h"
#include "card.hpp"
#include "game.h"
#include "connection.h"
#include "transpositionTable.h"

#include <array>
#include <string>




struct SearchResult : public RootResult {
	S64 durationUs;
};
struct SearchTimeResult : public SearchResult {
	Depth depth;
};



Board parseBoard(std::string str, bool flip, bool player, U8 cardI = 10);
CardsInfo parseCards(std::array<std::string, 5> cardNames, bool flipped);

class Game {
public:
	// game.cpp
	Game(std::array<std::string, 5> cardNames, std::string boardString = "1121100000000000000033433", bool player = 0, bool flipped = false);
	Game(const CardsInfo* cards, const Board& board = parseBoard("1121100000000000000033433", false, 0), bool player = 0, bool p0Turn = 1);
	Game(const Connection::LoadResult loadResult);
	Game(Connection& connection);

	void waitTurn(Connection& connection);
	void submitMove(Connection& connection, Board& board);


	const CardsInfo* cards;
	TranspositionTableWrapper tt;

	bool player = 0;
	bool myTurn = 1;
	bool ended = false;
	// U32 turnCount = 0;
	// U8 lastDepth = 0;
	// Score lastScore = 0;
	Board board;


	// gameSearch.cpp
private:
	SearchResult search(Depth depth, bool searchWin = false, Score alpha = SCORE::LOSS, Score beta = SCORE::WIN, bool print = true);
public:
	SearchTimeResult searchTime(S64 timeMs, Depth maxDepth = DEPTH_MAX, Score lastScore = 0, Depth lastDepth = 1);
	// SearchTimeResult searchTimeWithPanic(Game& game, S64 timeMs, SearchTimeResult& lastResult);
};