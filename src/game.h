#pragma once

#include "board.h"
#include "card.hpp"
#include "game.h"
#include "connection.h"

#include <array>
#include <string>



class Game {
private:
	Connection::LoadResult _loadResult;

public:
	Game(std::array<std::string, 5> cardNames, std::string boardString = "1121100000000000000033433", bool player = 0, bool flipped = false);
	Game(const CardsInfo& cards, const Board& board = Connection::parseBoard("1121100000000000000033433", false), bool player = 0);
	Game(Connection& connection);
private:
	void init();
public:
	bool myTurn = 1;
	// U32 turnCount = 0;
	// U8 lastDepth = 0;
	// Score lastScore = 0;
	CardsInfo cards;
	Board board;
};