#pragma once

#include "board.h"
#include "card.hpp"
#include "game.h"
#include "connection.h"
#include "transpositionTable.h"

#include <array>
#include <string>



class Game {
public:
	Game(std::array<std::string, 5> cardNames, std::string boardString = "1121100000000000000033433", bool player = 0, bool flipped = false);
	Game(const CardsInfo* cards, const Board& board = Connection::parseBoard("1121100000000000000033433", false, 0), bool player = 0, bool myTurn = 1);
	Game(const Connection::LoadResult loadResult);
	Game(Connection& connection);


	const CardsInfo* cards;
	TranspositionTableWrapper tt;

	bool player = 0;
	bool myTurn = 1;
	bool ended = false;
	// U32 turnCount = 0;
	// U8 lastDepth = 0;
	// Score lastScore = 0;
	Board board;
};