#include "game.h"

// leak some memory, whatever
Game::Game(std::array<std::string, 5> cardNames, std::string boardString, bool player, bool flipped) :
	Game(new CardsInfo(Connection::parseCards(cardNames, flipped)), Connection::parseBoard(boardString, flipped, player), player) { }

Game::Game(Connection& connection) : Game(connection.load()) { }

Game::Game(const Connection::LoadResult loadResult) : cards(new CardsInfo(loadResult.cards)), board(loadResult.board), myTurn(loadResult.myTurn), player(loadResult.player) {
	init();
}

Game::Game(const CardsInfo* cards, const Board& board, bool player, bool myTurn) : cards(cards), board(board), myTurn(myTurn) {
	init();
}

void Game::init() {
// #ifdef USE_TB
// 	tableBase.generate(*this);
// #endif
// #ifdef USE_TT
// 	transpositionTable.init();
// #endif
}
