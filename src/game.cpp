#include "game.h"


Game::Game(std::array<std::string, 5> cardNames, std::string boardString, bool player, bool flipped) :
	Game(Connection::parseCards(cardNames, flipped), Connection::parseBoard(boardString, flipped), player) { }

Game::Game(Connection& connection) : _loadResult(connection.load()), cards(_loadResult.cards), board(_loadResult.board) {
	init();
}

Game::Game(const CardsInfo& cards, const Board& board, bool player) : cards(cards), board(board), myTurn(!player) {
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
